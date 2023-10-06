/**
 * MMU管理模块API实现
 */
#include "globaltype.h"
#include "globalctrl.h"


/**
 * 1. 注意所有区分地址和指针，地址都使用addr_t类型，地址可以有虚拟地址和物理地址，但是指针一律使用虚拟地址
 * 2. x86 CPU的内存管理单元（MMU）可以同时支持两种分页模式：2MB分页模式 & 4KB分页模式
 *      2MB分页模式：通过将连续的2MB虚拟地址空间映射到连续的2MB物理地址空间来提高内存访问效率。
 *  这种模式适用于需要大块连续内存的应用程序，如大型数据库或图形处理
 *      4KB分页模式：将4KB虚拟地址空间映射到4KB的物理地址空间。
 *  这种模式适用于大多数应用程序，应为它提供了更细粒度的内存管理和更灵活的地质映射。
 *      在实际应用中，操作系统可以根据需要选择使用2MB分页模式或4K模式。
 *  例如，操作系统可以使用2MB分页模式来映射内核空间，以提高内核的访问效率。
 *  同时他可以使用4KB分页模式来映射用户空间，以提供更细粒度的内存管理和更好的隔离性。
 */



/**
 * 创建一个一级页目录
 * 创建过程是申请一个页面，获取MemPage，将对应物理页初始化为页表，然后返回对应的页描述符
 */
static MemPage* new_pt1(mmu_t *mmu){
	//参数检查
	if(NULL==mmu){
		return NULL;
	}
	
	//申请一个物理内存页面
	u64_t realNumber = 0;
	MemPage *page = mem_divide(&memmgr,1,&realNumber,MEMAREA_TYPE_KRNL,DMF_RELDIV);
	if(NULL==page){
		return NULL;	
	}

	//从页描述符中获取页虚拟地址，并将该页初始化为页目录表
	pt1_t *pt1 = (pt1_t*)mempage_vadr(page);
	pt1_init(pt1);

	//将页描述符挂载到mmu对应链表中，以表示分配关系
	list_add(&page->hook,&mmu->pt1s);
	mmu->pt1sNum++;

	//顶级页目录独有字段
	mmu->pt1 = pt1;

	return page; 
}
static MemPage* new_pt2(mmu_t *mmu){
	//参数检查
	if(NULL==mmu){
		return NULL;
	}

	size_t realNumber = 0;
	MemPage *page = mem_divide(&memmgr,1,&realNumber,MEMAREA_TYPE_KRNL,DMF_RELDIV);
	if(NULL==page){
		return FALSE;
	}

	pt2_t *pt2 = (pt2_t*)mempage_vadr(page);
	pt2_init(pt2);

	list_add(&page->hook,&mmu->pt2s);
	mmu->pt2sNum++;
	
	return page; 
}
static MemPage* new_pt3(mmu_t *mmu){
	if(NULL==mmu){
		return NULL;
	}

	size_t realNumber = 0;
	MemPage *page = mem_divide(&memmgr,1,&realNumber,MEMAREA_TYPE_KRNL,DMF_RELDIV);
	if(NULL==page){
		return NULL;
	}

	pt3_t *pt3 = (pt3_t*)mempage_vadr(page);
	pt3_init(pt3);

	list_add(&page->hook,&mmu->pt3s);
	mmu->pt3sNum++;
	
	return page; 
}
static MemPage* new_pt4(mmu_t *mmu){
	if(NULL==mmu){
		return NULL;
	}

	size_t realNumber = 0;
	MemPage *page = mem_divide(&memmgr,1,&realNumber,MEMAREA_TYPE_KRNL,DMF_RELDIV);
	if(NULL==page){
		return NULL;
	}

	pt4_t *pt4 = (pt4_t*)mempage_vadr(page);
	pt4_init(pt4);

	list_add(&page->hook,&mmu->pt4s);
	mmu->pt4sNum++;
	
	return page;
}

/**
 * 删除一个一级页目录
 * mmu: mmu管理结构
 * pt1：页表指针
 * page: 页表占用物理页的页描述符
 * 在执行删除时，如果直接指明了page，而且这个page就是pt1页表占用物理页的描述符的话，
 * 则可以直接使用list_del从链表删除，而不必再去mmu->pt1s链表中再去查找。可以大大提高删除速度
 */
static bool_t del_pt1(mmu_t *mmu,pt1_t *pt1,MemPage *page){
	//参数检查
	if(NULL==mmu || NULL==pt1){
		return FALSE;
	}
	
	//获取物理地址
	addr_t padr = MAP_VR((addr_t)pt1); 
	
	if(NULL!=page){	//指明了待释放的MemPage
		if(mempage_padr(page)==padr){ 	//这个page正好就是我们要释放的页表占用的page
			//从mmu链表中移除page，归还物理页面，计数减一
			list_del(&page->hook);	//页描述符必须先脱链，才能释放，因为释放时也要修改hook
			if(!mem_merge(&memmgr,page,mempage_len(page))){
				printk("error in merge page\n");
				return FALSE;
			}
			mmu->pt1sNum--;	//计数减1为什么放在释放页之后？？？
			return TRUE;		
		}
	}

	//如果page为NULL，只能根据pt1的地址去查找并删除
	List *pos = NULL;
	list_for_each(pos,&mmu->pt1s){
		page = list_entry(pos,MemPage,hook);
		if(mempage_padr(page)==padr){
			list_del(&page->hook);
			if(!mem_merge(&memmgr,page,mempage_len(page))){
				printk("error in merge page\n");
				return FALSE;
			}
			mmu->pt1sNum--;
			return TRUE;	
		}
	}
	//查找失败：删除失败
	return FALSE;
}
static bool_t del_pt2(mmu_t *mmu,pt2_t *pt2,MemPage *page){
	if(NULL==mmu || NULL==pt2){
		return FALSE;
	}

	addr_t padr = MAP_VR((addr_t)pt2);
	if(NULL!=page){
		if(mempage_padr(page)==padr){
			list_del(&page->hook);
			if(!mem_merge(&memmgr,page,mempage_len(page))){
				printk("error in merge page\n");
				return FALSE;
			}
			mmu->pt2sNum--;
			return TRUE;	
		}
	}
	
	List *pos = NULL;
	list_for_each(pos,&mmu->pt2s){
		page = list_entry(pos,MemPage,hook);
		if(mempage_padr(page)==padr){
			list_del(&page->hook);
			if(!mem_merge(&memmgr,page,mempage_len(page))){
				printk("error in merge page\n");
				return FALSE;
			}
			mmu->pt2sNum--;
			return TRUE;	
		}
	}
	
	return FALSE;
}
static bool_t del_pt3(mmu_t *mmu,pt3_t *pt3,MemPage *page){
	if(NULL==mmu||NULL==pt3){
		return FALSE;
	}

	addr_t padr = MAP_VR((addr_t)pt3);
	if(NULL!=page){
		if(padr==mempage_padr(page)){
			list_del(&page->hook);
			if(!mem_merge(&memmgr,page,mempage_len(page))){
				printk("error in merge page\n");
				return FALSE;
			}
			mmu->pt3sNum--;
			return TRUE;
		}
	}

	List *pos = NULL;
	list_for_each(pos,&mmu->pt3s){
		page = list_entry(pos,MemPage,hook);
		if(padr==mempage_padr(page)){
			list_del(&page->hook);
			if(!mem_merge(&memmgr,page,mempage_len(page))){
				printk("error in merge page\n");
				return FALSE;
			}
			mmu->pt3sNum--;
			return TRUE;
		}
	}
	
	return FALSE;
}
static bool_t del_pt4(mmu_t *mmu,pt4_t *pt4,MemPage *page){
	if(NULL==mmu||NULL==pt4){
		return FALSE;
	}

	addr_t padr = MAP_VR((addr_t)pt4);
	if(NULL!=page){
		if(padr==mempage_padr(page)){
			list_del(&page->hook);
			if(!mem_merge(&memmgr,page,mempage_len(page))){
				printk("error in merge page\n");
				return FALSE;
			}
			mmu->pt4sNum--;
			return TRUE;
		}
	}

	List *pos = NULL;
	list_for_each(pos,&mmu->pt4s){
		page = list_entry(pos,MemPage,hook);
		if(padr==mempage_padr(page)){
			list_del(&page->hook);
			if(!mem_merge(&memmgr,page,mempage_len(page))){
				printk("error in merge page\n");
				return FALSE;
			}
			mmu->pt4sNum--;
			return TRUE;
		}
	}
	return FALSE;
}


/**
 * 完成虚拟地址性物理地址转换需要的页表结构
 */
static pt2_t* transform_pt1(mmu_t *mmu,pt1_t *pt1,addr_t vadr,u64_t flags,MemPage **page){
	//参数检查
	if(NULL==mmu || NULL==pt1 || NULL == page){
		return NULL;
	}
	printk("m:1.1.0\n");
	//从虚拟地址中获取一级页表索引
	uint_t idx = pt1_idx(vadr);
	printk("m:1.1.1\n");
	//根据索引查询一级页表获取页表项
	pt1entry_t entry = pt1->entries[idx];

	//检查页表项是否为空
	if(pt1entry_is_have(&entry)==TRUE){
		//如果一级页表项非空，也就是指向一个二级页表的话，无需新建页表
		*page = NULL;
		return pt1entry_get_pt2(&entry);
	}//else
	printk("m:1.1.2\n");
	//页表项为空，需要新建二级页表
	*page = new_pt2(mmu);
	if(NULL==*page){
		printk("m:1.1.2.\n");
		return NULL;
	}
	printk("m:1.1.3\n");
	//获取新建二级页表的物理地址
	addr_t padr = mempage_padr(*page);
	//将二级页表地址和标志写入一级页表项
	pt1->entries[idx].entry = ((u64_t)padr) | flags;
	printk("m:1.1.4\n");	
	return (pt2_t*)MAP_RV(padr);	//返回二级页表指针
}
static pt3_t* transform_pt2(mmu_t *mmu,pt2_t *pt2,addr_t vadr,u64_t flags,MemPage **page){
	if(NULL==mmu || NULL==pt2 || NULL==page){
		return NULL;
	}

	uint_t idx = pt2_idx(vadr);
	pt2entry_t entry = pt2->entries[idx];

	if(pt2entry_is_have(&entry)==TRUE){
		*page = NULL;
		return pt2entry_get_pt3(&entry); 
	}

	*page = new_pt3(mmu);
	if(NULL==*page){
		return NULL;
	}

	addr_t padr = mempage_padr(*page);
	pt2->entries[idx].entry = ((u64_t)padr) | flags;

	return (pt3_t*)MAP_RV(padr);
}
static pt4_t* transform_pt3(mmu_t *mmu,pt3_t *pt3,addr_t vadr,u64_t flags,MemPage **page){
	if(NULL==mmu || NULL==pt3 || NULL == page){
		return NULL;	
	}

	uint_t idx = pt3_idx(vadr);
	pt3entry_t entry = pt3->entries[idx];

	if(pt3entry_is_have(&entry)==TRUE){
		*page = NULL;
		return pt3entry_get_pt4(&entry);
	}

	*page = new_pt4(mmu);
	if(NULL==*page){
		return NULL;
	}

	addr_t padr = mempage_padr(*page);
	pt3->entries[idx].entry = ((u64_t)padr) | flags;
	printk("padr:0x%lx\n",padr);
	return (pt4_t*)MAP_RV(padr);
}
static bool_t transform_pt4(mmu_t *mmu,pt4_t* pt4,addr_t vadr,addr_t padr,u64_t flags){
	if(NULL==mmu || NULL==pt4){
		return FALSE;
	}
	printk("m:1.4.0\n");
	//直接最终物理页地址写入四级页表
	uint_t idx = pt4_idx(vadr);
	printk("m:1.4.1\n");
	pt4->entries[idx].entry = ((u64_t)padr) | flags;
	printk("m:1.4.2\n");
	return TRUE;
}

/**
 * 解除地址映射
 */
static addr_t untransform_pt4(mmu_t *mmu,pt4_t *pt4,addr_t vadr){
	if(NULL==mmu || NULL==pt4){
		return NULL;
	}

	uint_t idx = pt4_idx(vadr);
	pt4entry_t entry = pt4->entries[idx];

	if(pt4entry_is_have(&entry)==FALSE){
		return NULL;
	}

	//清空四级页表项
	pt4->entries[idx].entry = 0;
	//返回最终的页物理地址
	return pt4entry_padr(&entry);
}
static bool_t untransform_pt3(mmu_t *mmu,pt3_t *pt3,addr_t vadr,MemPage *page){
	if(NULL==mmu || NULL==page){
		return FALSE;
	}

	uint_t idx = pt3_idx(vadr);
	pt3entry_t entry = pt3->entries[idx];

	if(pt3entry_is_have(&entry)==FALSE){
		return TRUE;
	}

	pt4_t *pt4 = pt3entry_get_pt4(&entry);
	if(pt4_is_empty(pt4)==FALSE){
		return TRUE;
	}

	if(del_pt4(mmu,pt4,page)==FALSE){
		return FALSE;
	}

	pt3->entries[idx].entry = 0;
	
	return TRUE;
}
static bool_t untransform_pt2(mmu_t *mmu,pt2_t *pt2,addr_t vadr,MemPage *page){
	if(NULL==mmu || NULL==pt2){
		return FALSE;
	}

	uint_t idx = pt2_idx(vadr);
	pt2entry_t entry = pt2->entries[idx];

	if(pt2entry_is_have(&entry)==FALSE){
		return TRUE;
	}

	pt3_t *pt3 = pt2entry_get_pt3(&entry);
	if(pt3_is_empty(pt3)==FALSE){
		return FALSE;
	}

	if(del_pt3(mmu,pt3,page)==FALSE){
		return FALSE;
	}

	pt2->entries[idx].entry = 0;
	
	return TRUE;
}
static bool_t untransform_pt1(mmu_t *mmu,pt1_t *pt1,addr_t vadr,MemPage *page){
	//参数检查
	if(NULL==mmu || NULL==pt1){
		return FALSE;
	}
	
	//从虚拟地址中提取一级页表索引，随后查询一级页表，获取一级页表项
	uint_t idx = pt2_idx(vadr);
	pt1entry_t entry = pt1->entries[idx];

	//检查这个页表项是否存在映射，如果没有，则提前结束工作
	if(pt1entry_is_have(&entry)==FALSE){
		return TRUE;
	}

	//从页表项中获取下一级页表
	pt2_t *pt2 = pt1entry_get_pt2(&entry);
	//检查下一级页表是否全空
	if(pt2_is_empty(pt2)==FALSE){
		//2级页表还有页表项，不删除该页表，工作提前结束
		return TRUE;
	}

	//2级页表为空表，删除该表
	if(del_pt2(mmu,pt2,page)==FALSE){
		return FALSE;
	}

	//删除页表成功，清空该页表项
	pt1->entries[idx].entry = 0;
	
	return TRUE;
}


static bool_t mmu_transform_core(mmu_t *mmu,addr_t vadr,addr_t padr,u64_t flags){
	bool_t rets = FALSE;

	spinlock_lock(&mmu->lock);
	printk("m:1.0\n");
	//获取一级页表
	pt1_t *pt1 = mmu->pt1;
	if(NULL==pt1){
		rets = FALSE;
		goto out;
	}
	printk("m:1.1\n");
	//一级页表映射
	MemPage *page2 = NULL;
	pt2_t *pt2 = transform_pt1(mmu,pt1,vadr,flags,&page2);
	if(NULL==pt2){
		printk("m:1.1.\n");
		rets = FALSE;
		goto out1;
	}
	printk("m:1.2\n");
	//二级页表映射
	MemPage *page3 = NULL;
	pt3_t *pt3 = transform_pt2(mmu,pt2,vadr,flags,&page3);
	if(NULL==pt3){
		rets = FALSE;
		goto out2;
	}
	printk("m:1.3\n");
	//三级页表映射
	MemPage *page4 = NULL;
	pt4_t *pt4 = transform_pt3(mmu,pt3,vadr,flags,&page4);
	if(NULL==pt4){
		rets = FALSE;
		goto out3;
	}
	printk("m:1.4\n");
	//四级页表映射
	rets = transform_pt4(mmu,pt4,vadr,padr,flags);
	printk("m:1.5\n");
	if(FALSE==rets){
		rets = FALSE;
		goto out4;
	}
	printk("m:1.6\n");
	//映射成功
	rets = TRUE;
	mmu_refresh();	//为什么建立完页表之后需要刷新cr3寄存器
	goto out;
	
out4:
	untransform_pt4(mmu,pt4,vadr);
out3:
	untransform_pt3(mmu,pt3,vadr,page4);
out2:
	untransform_pt2(mmu,pt2,vadr,page3);
out1:
	untransform_pt1(mmu,pt1,vadr,page2);
out:
	spinlock_unlock(&mmu->lock);
	return rets;
}


/**
 * mmu地址映射函数：建立从vadr到padr映射的各级页表
 * mmu：MMU管理结构体
 * vadr: 待转换的虚拟地址
 * padr: 转换到的物理地址
 * flags: 建立初始页表项所需的标志
 */
bool_t mmu_transform(mmu_t *mmu,addr_t vadr,addr_t padr,u64_t flags){
	//参数检查
	printk("m:0\n");
	if(NULL==mmu){
		return FALSE;
	}
	printk("m:1\n");
	return mmu_transform_core(mmu,vadr,padr,flags);
}


/**
 * 根据虚拟地址，提取页表索引，查询对应页表，获取下一级页表指针
 */
static pt2_t* pt1_find_pt2(pt1_t *pt1,addr_t vadr){
	//1.参数检查
	if(NULL==pt1){
		return NULL;
	}
	//2.从虚拟地址中获取一级页表索引
	uint_t idx = pt1_idx(vadr);

	//3.根据索引查询一级页表获取页表项
	pt1entry_t entry = pt1->entries[idx];

	//4.检查页表项是否为空
	if(FALSE == pt1entry_is_have(&entry)){
		return NULL;
	}

	//5.检查完毕,从一级页表项中获取二级页表并返回
	return pt1entry_get_pt2(&entry);
}
static pt3_t* pt2_find_pt3(pt2_t *pt2,addr_t vadr){
	if(NULL==pt2){
		return NULL;
	}

	uint_t idx = pt2_idx(vadr);

	pt2entry_t entry = pt2->entries[idx];

	if(FALSE==pt2entry_is_have(&entry)){
		return NULL;
	}
	
	return pt2entry_get_pt3(&entry);
}
static pt4_t* pt3_find_pt4(pt3_t *pt3,addr_t vadr){
	if(NULL==pt3){
		return NULL;
	}

	uint_t idx = pt3_idx(vadr);

	pt3entry_t entry = pt3->entries[idx];

	if(FALSE==pt3entry_is_have(&entry)){
		return FALSE;
	}
	
	return pt3entry_get_pt4(&entry);
}
static addr_t pt4_find_adr(pt4_t *pt4,addr_t vadr){
	if(NULL==pt4){
		return NULL;
	}

	uint_t idx = pt4_idx(vadr);
	
	pt4entry_t entry = pt4->entries[idx];

	if(FALSE != pt4entry_is_have(&entry)){
		return NULL;
	}
	//从四级页表中查找页表项，获取最终的页物理地址
	return pt4entry_padr(&entry);
}


static addr_t mmu_untransform_core(mmu_t *mmu,addr_t vadr){
	addr_t rets = NULL; 
	spinlock_lock(&mmu->lock);
	
	//从一级页表中获取二级页表
	pt2_t *pt2 = pt1_find_pt2(mmu->pt1,vadr);
	if(NULL==pt2){
		rets = NULL;
		goto out;
	}

	//从二级页表中获取三级页表
	pt3_t *pt3 = pt2_find_pt3(pt2,vadr); 
	if(NULL==pt3){
		rets = NULL;
		goto out1;
	}

	//从三级页表中获取四级页表
	pt4_t *pt4 = pt3_find_pt4(pt3,vadr);
	if(NULL==pt4){
		rets = NULL;
		goto out2;
	}
	
	rets = untransform_pt4(mmu,pt4,vadr);

	untransform_pt3(mmu,pt3,vadr,NULL);
out2:
	untransform_pt2(mmu,pt2,vadr,NULL);
out1:
	untransform_pt1(mmu,mmu->pt1,vadr,NULL);
out:
	spinlock_unlock(&mmu->lock);
	return rets;
}


/**
 * mmu取消地址映射函数：取消虚拟地址vadr的映射
 * vadr: 待取消的虚拟地址
 * return：
 */
addr_t mmu_untransform(mmu_t *mmu,addr_t vadr){
	//参数检查
	if(NULL==mmu){
		return EPARAM;
	}
	return mmu_untransform_core(mmu,vadr);
}


/**
 * 删除并归还同一级别所有页表占用的物理内存页
 */
static bool_t clean_pt1(mmu_t *mmu){
	//参数检查
	if(NULL==NULL){
		return FALSE;
	}
	//归还一级页表占用的所有物理页
	List *pos = NULL;
	MemPage *page = NULL;
	list_for_each_head_dell(pos,&mmu->pt1s){
		//页描述符脱链
		page = list_entry(pos,MemPage,hook);
		list_del(&page->hook);
		//归还物理页
		if(!mem_merge(&memmgr,page,mempage_len(page))){
			printk("error in clean_pt1\n");
			die(0);
		}
		//页描述符计数-1
		mmu->pt1sNum--;
	}
	
	return TRUE;
}
static bool_t clean_pt2(mmu_t *mmu){
	if(NULL==mmu){
		return FALSE;
	}

	List *pos = NULL;
	MemPage *page = NULL;
	list_for_each_head_dell(pos,&mmu->pt2s){
		page = list_entry(pos,MemPage,hook);
		list_del(&page->hook);
		if(!mem_merge(&memmgr,page,mempage_len(page))){
			printk("error in clean_pt1\n");
			die(0);
		}
		mmu->pt2sNum--;
	}
	
	return TRUE;
}
static bool_t clean_pt3(mmu_t *mmu){
	if(NULL==mmu){
		return FALSE;
	}

	List *pos = NULL;
	MemPage *page = NULL;
	list_for_each_head_dell(pos,&mmu->pt3s){
		page = list_entry(pos,MemPage,hook);
		list_del(&page->hook);
		if(!mem_merge(&memmgr,page,mempage_len(page))){
			printk("error in clean_pt1\n");
			die(0);
		}
		mmu->pt3sNum--;
	}
	
	return TRUE;
}
static bool_t clean_pt4(mmu_t *mmu){
	if(NULL==mmu){
		return FALSE;
	}

	List *pos = NULL;
	MemPage *page = NULL;
	list_for_each_head_dell(pos,&mmu->pt4s){
		page = list_entry(pos,MemPage,hook);
		list_del(&page->hook);
		if(!mem_merge(&memmgr,page,mempage_len(page))){
			printk("error in clean_pt1\n");
			die(0);
		}
		mmu->pt4sNum--;
	}
	
	return TRUE;
}


/**
 * 删除并归还页表占用的所有内存空间
 */
bool_t mmu_clean(mmu_t *mmu){
	bool_t rets = FALSE;
	cr3_t cr3;
	//参数检查
	if(NULL==mmu){
		return FALSE;
	}

	spinlock_lock(&mmu->lock);
	
	//读取cr3寄存器，并获取一级页表的物理地址(pcr3)和虚拟地址(vcr3)
	cr3.entry = (u64_t)read_cr3();
	addr_t pcr3 = (addr_t)(cr3.flags.adr<<12);
	addr_t vcr3 = MAP_RV(pcr3);

	//why？？？？？
	if(vcr3 == (addr_t)mmu->pt1 ){
		return FALSE;
		goto out;
	}

	//依次从mmu中删除并归还各级页表占用的物理页
	if(clean_pt1(mmu)==FALSE){
		rets = FALSE;
		goto out;
	}
	if(clean_pt2(mmu)==FALSE){
		rets = FALSE;
		goto out;
	}
	if(clean_pt3(mmu)==FALSE){	
		rets = FALSE;
		goto out;
	}
	if(clean_pt4(mmu)==FALSE){
		rets = FALSE;
		goto out;
	}
	//完成全部处理，返回TRUE
	rets = TRUE;
out:
	spinlock_unlock(&mmu->lock);
	return rets;
}


/**
 * 加载mmu页表
 */
void mmu_load(mmu_t *mmu){
	//参数检查
	if(NULL==mmu){
		return;
	}

	spinlock_lock(&mmu->lock);
	//检查是否存在一级页表，如果存在，检查一级页表是否4K对齐
	if(NULL==mmu->pt1 || 0 != (((addr_t)mmu->pt1)&0xfff)){
		goto out;
	}

	//将一级页表物理地址写入cr3寄存器（注意cr3寄存器存的必须是一级页表物理地址）
	cr3_t cr3;
	cr3.entry = MAP_VR((addr_t)mmu->pt1);
	write_cr3(cr3.entry);

out:
	spinlock_unlock(&mmu->lock);
	return;
}


/**
 * 读出cr3寄存器值，并重新写入
 */
void mmu_refresh(){
	cr3_t cr3;
	//读取cr3寄存器
	cr3.entry = (u64_t)read_cr3();
	//写入cr3寄存器
	write_cr3(cr3.entry);
	return;
}


/**
 * 初始化mmu
 */
bool_t mmu_init(mmu_t *mmu){
	bool_t rets = FALSE;
	cr3_t cr3;
	
	//参数检查
	if(NULL==mmu){
		return FALSE;
	}

	//初始化mmu结构体
	mmu_t_init(mmu);
	
	spinlock_lock(&mmu->lock);

	//为mmu创建一个一级页表
	if(NULL==new_pt1(mmu)){
		rets = FALSE;
		goto out;
	}

	/**
 	 * 保留原先设置的2MB模式页表，并取消平坦映射
 	 */
	//1.读取原先的cr3寄存器，并获取顶级页表的物理地址和线性虚拟地址
	cr3.entry = read_cr3();
	addr_t pcr3 = (addr_t)(cr3.flags.adr<<12);
	addr_t vcr3 = MAP_RV(pcr3);
	//2.将原先的一级页表复制到受mmu管理一级页表下
	memcpy(vcr3,sizeof(pt1_t),(addr_t)mmu->pt1);
	//3.将mmu管理的一级页表设置到cr3寄存器中
	cr3.entry = (u64_t)MAP_VR((addr_t)mmu->pt1);
	//4.取消原先的平坦映射，用于用户地址空间映射（这个时候再使用平坦映射将会产生缺页异常）
	mmu->pt1->entries[0].entry = 0;

	rets = TRUE;

out:
	spinlock_unlock(&mmu->lock);
	return rets;
}


//=============================================================================================================
INLINE void test_transform();
INLINE void test_untransform();
INLINE void test_mmu_init();
INLINE void test_mmu_refresh();
INLINE void test_clean();
INLINE void test_new_ptx();
INLINE void test_del_ptx();
INLINE void test_find();

void mmu_test_main(){
	test_transform();


	die(0);
	return;
}

INLINE void test_transform(){
	
	return;
}

INLINE void test_untransform(){
	
	return;
}

INLINE void test_mmu_init(){
	return;
}

INLINE void test_mmu_refresh(){
	return;
}

INLINE void test_clean(){
	
}

INLINE void test_new_ptx(){
	return;
}

INLINE void test_del_ptx(){
	return;
}

INLINE void test_find(){
	return;
}

/**
 * 测试内置函数
 */
