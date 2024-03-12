/**
 * 
 */
#include "globaltype.h"
#include "globalctrl.h"

/**
 * 接口函数负责参数检查，核心函数强调具体步骤，以及每步的合法性检查
 */


/**
 * 初始化内核虚拟内存地址空间
 */
void kvm_init(){
	//初始化进程的用户空间
	init_userspace_kvmspace(&kvmProcess.kvmSpace);
	
	//初始化mmu页表
	mmu_init(&kvmProcess.mmu);
	
	//加载mmu页表
	mmu_load(&kvmProcess.mmu);

	//初始化内核内存池
	mempool_init();
	
	return;
}

void kvmProcess_init(KvmProcess *process){
	spinlock_init(&process->lock);
	list_t_init(&process->hook);
	process->flag = 0;
	process->status = 0;
	process->count = 0;		//KvmProcess的共享计数
	
	/**
	 * 初始化Sem,MMU,KvmSpace
	 */
	//krlsem_t_init(&initp->msd_sem);
	//krlsem_set_sem(&initp->msd_sem, SEM_FLG_MUTEX, SEM_MUTEX_ONE_LOCK);
	mmu_t_init(&process->mmu);
	kvmSpace_init(&process->kvmSpace);

	process->stext = 0;
	process->etext = 0;
	process->sdata = 0;
	process->edata = 0;
	process->sbss = 0;
	process->ebss = 0;
	process->sheap = 0;
	process->eheap = 0;
	
	return;
}


/**
 * 检查当前区域后面的空闲虚拟内存空间是否可用
 */
static bool_t check_area_for_new(KvmSpace *space,KvmArea *area,addr_t start,size_t size){
	addr_t end = start+size;		//统一计算待检查内存区域的end值
	addr_t empty_end;			//空闲内存区的[empty_start,empty_end)，省略了empty_start

	//area是链表尾节点:(cur,leisure)
	if(list_is_last(&area->hook,&space->areas)){
		empty_end = space->freeEnd;
	}else{	//area不是链表尾节点:(cur,leisure,next)
		//获取area后面的下一个KvmArea结构
		KvmArea *nextArea = list_next_entry(area,KvmArea,hook);
		empty_end = nextArea->start;	//将下一个Area的起始地址作为空闲区域的上限
	}

	/**
	 * 空闲空间的empty_start=area->end(省略),而empty_end上面已经讨论，下面判断(start,size,end)是否合法
	 */
	if(NULL==start){	//用户未指定开始地址：只需要保证空闲地址空间够分配就行
		if(area->end+size<=empty_end){
			return TRUE;
		}
	}else{	//用户指定开始地址，需要保证空闲地址空间包含请求地址空间
		if(area->end<=start && (start+end)<=empty_end){
			return TRUE;
		}
	}
	
	return FALSE;
}


/**
 * 查找用于分配虚拟内存区间的空闲区，并返回这个空闲区前面的一个已分配的KvmArea
 * process: 
 * start: 
 * size: 页对齐后的size
 */
static KvmArea* find_area_for_new(KvmSpace *space,addr_t start,addr_t size){
	KvmArea *curArea = space->curArea;		//获取当前虚拟内存空间中最近使用的KvmArea
	printk("#1.1.1\n");
	//接口函数检查的size和end的是size未对齐之前的，因此在对齐之后还要进行重复检查(待定优化)
	if(size<0x1000 || (start+size)>space->freeEnd){
		return NULL;
	}
	printk("#1.1.2\n");
	//检查当前KvmArea是否可行（cache）
	if(NULL!=curArea){	//首次使用时curArea可能为NULL
		if(check_area_for_new(space,curArea,start,size)){
			return curArea;
		}
	}
	printk("#1.1.3\n");
	//cache不命中，依次遍历每个KvmArea
	list_t *pos = NULL;	//遍历工作指针
	list_for_each(pos,&space->areas){
		curArea = list_entry(pos,KvmArea,hook);	//获取链表上钩子pos挂载的结构体KvmArea
		//检查curArea是否可行(curArea不为NULL)
		if(check_area_for_new(space,curArea,start,size)){
			return curArea;
		}
	}
	printk("1.1.4\n");
	return NULL;	//查找失败
}


/**
 * 虚拟内存区间申请：核心函数
 * 
 */
static addr_t kvm_new_core(KvmProcess *process,addr_t start,size_t size,u64_t limits,u32_t mapType){
	addr_t retAdr = NULL;
	KvmSpace *space = &process->kvmSpace;	//从进程内存管理结构体获取虚拟内存空间结构体
	
	spinlock_lock(&space->lock);		//KvmSpace结构加锁
	//查找一个待分配内存空间的前一个KvmArea区间，并检查是否查找成功
	printk("#1.1\n");
	KvmArea *curArea = find_area_for_new(space,start,size);	
	if(NULL==curArea){
		retAdr = NULL;
		goto out;
	}
	printk("#1.2\n");
	/**
	 * 下面根据获取的内存区进行相关的处理
	 */
	//检查将要分配的虚拟内存区间是否可以合并到targetArea，以节省存储KvmArea结构的空间
	if((NULL==start||start==curArea->start) && limits==curArea->limits && mapType==curArea->mapType){
		retAdr = curArea->end;
		curArea->end += size;	//合并不能使用start，因为可能为NULL
		space->curArea = curArea;	//记录当前区间为目标区间
		goto out;
	}
	printk("#1.3\n");
	//不能复用的话，就建立一个新的KvmArea结构体
	KvmArea *newArea = new_kvmArea();
	if(NULL==newArea){	//检查是否创建成功
		retAdr = NULL;
		goto out;
	}

	//如果没有指定起始地址，则由系统自动分配
	if(NULL==start){
		//当然是接着当前虚拟地址区间之后开始
		newArea->start = curArea->end;
	}else{
		newArea->start = start;
	}

	//设置结束地址和标志
	newArea->end = newArea->start+size;
	newArea->limits = limits;
	newArea->mapType = mapType;
	newArea->father = space;

	//将newArea添加到curArea后面
	list_add(&newArea->hook,&curArea->hook);	//前插法
	if(list_is_last(&newArea->hook,&space->areas)){	//如果newArea是最后一个Area，则更新space->endArea
		space->endArea = curArea;
	}
	retAdr = newArea->start;
	
out:
	spinlock_unlock(&space->lock);		//KvmSpace结构解锁
	return retAdr;
}

/**
 * 虚拟内存空间申请：接口函数
 * process: 进程虚拟内存空间管理结构体
 * start: 申请的内存空间起始地址，如不指定起始地址则置为NULL（因为0地址被保留，所以不用担心起始地址被指定为NULL）
 * size：申请的虚拟内存空间大小：必须是页对齐的地址
 * limits:
 * type:
 * return：申请成功则返回虚拟内存首地址
 */
addr_t kvm_new(KvmProcess* process,addr_t start,size_t size,u64_t limits,uint_t mapType){
	//参数检查
	if(NULL==process||0==size){
		return NULL;
	}
	printk("#1\n");
	//如果用户指定了起始地址，那么这个起始地址要4K对齐
	if(NULL!=start){
		//4K对齐||第0页保留|| end超出用户空间
		if((start&0xfff)!=0 || start<0x1000 || (start+size)>USER_VIRADR_END){//(USER_VIRADR_END端点可取吗)
			return NULL;
		}
	}
	printk("#2\n");
	//完成参数检查
	return kvm_new_core(process,start,ALIGN_4K(size),limits,mapType);
}


/**
 * 查找用于分配虚拟内存区间的空闲区，并返回这个空闲区前面的一个已分配的KvmArea
 */
static KvmArea* find_area_for_delete(KvmSpace *space,addr_t start,size_t size){
	KvmArea *curArea = space->curArea;	//获取上一次使用的KvmArea
	addr_t end = start+size;

	if(NULL!=curArea){	//先检查上一次使用过的KvmArea是否匹配
		if(curArea->start<=start && end<=curArea->end){	//待释放区间包含在curArea区间中
			return curArea;
		}
	}

	//cache未命中，遍历space中的所有KvmArea
	list_t *pos = NULL;
	list_for_each(pos,&space->areas){
		curArea = list_entry(pos,KvmArea,hook);
		if(curArea->start<=start && end<=curArea->end){
			return curArea;
		}
	}
	
	return NULL;
}


/**
 * 删除用户进程中分配的虚拟内存区间：核心函数
 * process： 
 * start：
 * size：
 * 
 */
static bool_t kvm_delete_core(KvmProcess *process,addr_t start,size_t size){
	bool_t rets = FALSE;
	KvmSpace *space = &process->kvmSpace;	//从进程内存管理结构体中获取对应的内存空间结构体
	addr_t end = start+size;		//提前计算好待释放区域的上限

	spinlock_lock(&space->lock);		//KvmSpace结构加锁
	//查找目标区间，并检查是否查找成功
	KvmArea *curArea = find_area_for_delete(space,start,size);	
	if(NULL==curArea){
		rets = FALSE;
		goto out;
	}

	/**
	 * curArea包含待释放区间，即（curArea->start<=start && end<=curArea->end）
	 * 分以下四种情况：
	 * 1.完全相等：curArea->start == start && end == curArea->end
	 * 2.作为前缀：curArea->start == start && end <  curArea->end
	 * 3.作为后缀：curArea->start <  start && end == curArea->end
	 * 4.嵌入中间：curArea->start <  start && end <  curArea->end
	 */
	//1.完全相等：要释放的虚拟地址区间等于curArea
	if(curArea->start==start && end==curArea->end){	//直接将curArea脱链并释放
		list_del(&curArea->hook);		//将curArea从虚拟内存空间中删除
		space->areasNum--;		//虚拟内存空间的区间数-1
		del_kvmArea(curArea);	//释放curArea占用的内存对象空间	
		
		rets = TRUE;
		goto out;
	}
	//2.作为前缀：要释放的虚拟内存区间是curArea的前缀
	if(curArea->start==start && end<curArea->end){	//直接将end作为curArea->start
		curArea->start = end;
		rets = TRUE;
		goto out;
	}
	//3.作为后缀：要释放的虚拟内存区间是curArea的后缀
	if(curArea->start<start && end==curArea->end){	//直接将start最为curArea->end
		curArea->end = start;
		rets = TRUE;
		goto out;
	}
	//4.嵌入内部：要释放的虚拟内存区间处在curArea内部
	if(curArea->start<start && end<curArea->end){
		//申请一个KvmArea记录截断的后半部分，并检查是否申请成功
		KvmArea *newArea = new_kvmArea();
		if(NULL==newArea){
			rets = FALSE;
			goto out;
		}
		//修改区间范围
		newArea->start = end;
		newArea->end = curArea->end;
		curArea->end = start;
		//保持curArea与newArea其他字段的相同
		newArea->limits = curArea->limits;
		newArea->mapType = curArea->mapType;
		newArea->father = space;

		//将新建KvmArea加入链表，链表计数+1
		list_add(&newArea->hook,&curArea->hook);
		space->areasNum++;

		//newArea是否链表尾节点
		if(list_is_last(&newArea->hook,&space->areas)){
			space->endArea =newArea;		//space->endArea指向最新的尾节点
		}
		space->curArea = newArea;	//更新cache

		rets = TRUE;
		goto out;
	}
	
	//以上四种情况都不满足（除非出错才会出现这种情况）
	rets = FALSE;
	
out:
	spinlock_unlock(&space->lock);		//KvmSpace结构解锁
	return rets;
}


/**
 * 释放用户进程中分配的虚拟内存区间：接口函数
 * process:
 * start:
 * size：申请的虚拟内存空间大小：必须是页对齐的地址
 * return: 仅当释放成功时返回TRUE
 */
bool_t kvm_delete(KvmProcess *process,addr_t start,size_t size){
	//参数检查
	if(NULL==process || NULL==start || 0==size){
		return FALSE;
	}
	//参数没有参数错误
	return kvm_delete_core(process,start,ALIGN_4K(size));
}


//===================================================================================================================

/**
 * 在虚拟地址空间中检查缺页地址是否合法
 * 也就是检查这个地址是否属于内存空间中的某个内存区间
 */
static KvmArea* vma_map_find_kvmarea(KvmSpace *space,addr_t vadrs){
	KvmArea *cur = space->curArea;	//获取上次使用的Area
	//查看虚拟地址vadrs是否落在cur对应的内存区间中
	if(NULL!=cur){
		if(cur->start<=vadrs && vadrs<cur->end){
			return cur;
		}
	}

	//cache不命中，遍历所有的KvmArea(curArea会被重复判断)
	list_t *pos = NULL;
	list_for_each(pos,&space->areas){
		cur = list_entry(pos,KvmArea,hook);
		if(cur->start<=vadrs && vadrs<cur->end){
			return cur;
		}
	}
	//查找失败：vadrs落在未分配内存区中
	return NULL;
}

/**
 * 建立内存盒子结构
 */
static KvmBox* vma_map_get_kvmbox(KvmArea *area){
	//如果已经存在box结构则直接返回，不用新建
	if(NULL!=area->box){
		return area->box;
	}

	//新建一个KvmBox，并将其添加到area中
	KvmBox *box = new_kvmBox();
	if(NULL==box){	//创建失败，内存空间不足
		return NULL;
	}
	area->box = box;
	
	return area->box;
}


/**
 * 分配一个物理内存页面，挂载到KvmBox中，并返回对应的MemPage结构
 */
static MemPage* vma_new_usermsa(KvmProcess *process,KvmBox *box){
	/**
	 * 为什么重复进行参数检查
	 */
	printk("f:2.1.4.0.1\n");
	if(NULL==process||NULL==box){
		return NULL;
	}

	printk("f:2.1.4.0.2\n");
	MemPage *page = mem_divide_apps(&memmgr);
	printk("f:2.1.4.0.3\n");
	if(NULL==page){
		//内存分配失败
		return NULL;
	}

	//在内存共享时，同一个内存盒子可能被多个进程使用
	spinlock_lock(&box->lock);
	printk("f:2.1.4.0.4\n");
	//将分配的物理内存页挂载到KvmBox下，计数+1
	list_add(&page->hook,&box->pages);
	box->pagesNum++;

	spinlock_unlock(&box->lock);
	printk("f:2.1.4.0.5\n");
	return page;		
}

/**
 * 释放padr占用的一个用户空间内存页
 */
static bool_t vma_del_usermsa(KvmProcess *process,KvmBox *box,MemPage *page,addr_t padr){
	bool_t rets = FALSE;
	/**
	 * 为什么成功进行参数检查？
	 */
	if(NULL==process || NULL==box || NULL==page){
		return FALSE;
	}

	//如果提供了page，则可以直接将page脱链，而不必遍历寻找
	if(NULL!=page){
		//检查提供的MemPage是不是我们要释放的padr所在的页
		if(mempage_padr(page)==padr){
			//脱链，计数-1
			list_del(&page->hook);
			box->pagesNum--;
			rets = TRUE;
			goto out;
		}
	}

	//挨个遍历寻找
	list_t *pos = NULL;
	list_for_each(pos,&box->pages){
		page = list_entry(pos,MemPage,hook);
		if(mempage_padr(page)==padr){
			list_del(&page->hook);
			box->pagesNum--;
			rets = TRUE;
			goto out;
		}
	}

	rets = FALSE;
out:
	spinlock_unlock(&box->lock);
	//尝试归还内存页
	if(mem_merge(&memmgr,page,mempage_len(page))==FALSE){
		printk("error in merge memory\n");
		return FALSE;
	}
	return rets;
}


/**
 * 映射物理内存页面
 */
static addr_t vma_map_msa_fault(KvmProcess *process,KvmBox *box,addr_t vadr,u64_t flags){	
	//分配物理页面，挂载到box上，返回对应的MemPage结构
	printk("f:2.1.4.0\n");
	MemPage *userPage = vma_new_usermsa(process,box);
	if(NULL==userPage){
		//物理内存页分配失败
		return NULL;
	}
	printk("f:2.1.4.1\n");
	//获取分配的物理页物理地址，用于mmu映射
	addr_t padr = mempage_padr(userPage);
	printk("f:2.1.4.2\n");
	
	/**
	 * 空指针异常
	 */
	
	//进行mmu地址映射
	if(mmu_transform(&process->mmu,vadr,padr,flags)==TRUE){
		printk("2.1.4.3.");
		//映射成功，返回物理页地址
		return padr;
	}
	printk("f:2.1.4.3\n");
	//映射失败，释放申请的物理内存页
	vma_del_usermsa(process,box,userPage,padr);
	printk("f:2.1.4.4\n");
	return NULL;
}


/**
 * 缺页异常处理核心
 */
static sint_t vma_map_fairvadrs_core(KvmProcess *process,addr_t vadrs){
	sint_t rets = FALSE;
	printk("f:2.1.0\n");
	spinlock_lock(&process->lock);
	//查找虚拟地址所在的内存区间
	KvmArea *area = vma_map_find_kvmarea(&process->kvmSpace,vadrs);
	printk("f:2.1.1\n");
	if(NULL==area){
		rets = -EFAULT;
		goto out;
	}
	printk("f:2.1.2\n");
	//获取内存区间拥有的内存盒子
	KvmBox *box = vma_map_get_kvmbox(area);
	printk("f:2.1.3\n");
	if(NULL==box){
		rets = -ENOMEM;	//“内存空间不够”错误
		goto out;
	}
	printk("f:2.1.4\n");
	//分配物理内存页面，并建立mmu页表（映射完成后就存在于内存了，因此存在位置位）
	addr_t padrs = vma_map_msa_fault(process,box,vadrs,(0|MMU_PT4_US|MMU_PT4_RW|MMU_PT4_P));
	printk("f:2.1.5\n");
	if(NULL==padrs){
		rets = -ENOMEM;
		goto out;
	}
	
	printk("f:2.1.6\n");
	rets = EOK;
out:
	spinlock_unlock(&process->lock);
	return rets;
}


/**
 * 缺页异常处理接口
 * process: 待处理缺页异常进程的内存空间
 * vadrs: 触发缺页的虚拟地址
 */
static sint_t vma_map_fairvadrs(KvmProcess *process,addr_t vadrs){
	//参数检查:第0页保留|地址不可超过用户地址空间
	printk("f:2.0\n");
	if(vadrs<0x1000 || USER_VIRADR_END<vadrs){
		/**
		 * 为什么要使用相反数
		 */
		return -EPARAM;
	}
	printk("f:2.1\n");
	return vma_map_fairvadrs_core(process,vadrs);
}


/**
 * 异常分发器调用的接口
 * vadrs: 触发缺页的虚拟地址
 */
sint_t krluserspace_accessfailed(addr_t fairvadrs){
	//这里应该是获取当前进程的mm，但是由于现在没有进程，才用kvmProcess代替
	KvmProcess *process = &kvmProcess;	//获取进程管理结构体
	printk("f:1\n");
	//应用程序的虚拟地址不可超过总虚拟地址空间大小
	if(USER_VIRADR_END < fairvadrs){
		return -EACCES;
	}
	printk("f:2\n");
	return vma_map_fairvadrs(process,fairvadrs);
 }


//========================================================================================================
INLINE void test_kvm_new();
INLINE void test_kvm_delete();


void kvmProcess_test_main(){
	//test_kvm_new();
	//test_kvm_delete();	
	//分配一个大小为0x1000的虚拟内存空间
	addr_t adr = kvm_new(&kvmProcess,NULL,0x1000,0,0);
	if(NULL==adr){
		printk("fail to allocate kvm\n");
		die(0);
	}
	//输出开始地址
	printk("start:0x%lx\n", adr);	//分配的是虚拟地址
	printk("try to write data\n");
	memset((void*)adr,0,0x1000);
	printk("end writting data\n");
	kvm_delete(&kvmProcess,adr,0x1000);
	
	die(0);
	return ;
}


INLINE void test_kvm_new(){
	
	
	return;
}

INLINE void test_kvm_delete(){
	
	
	return;
}



