/**
 * 内存池模块
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"


/**
 * 初始化内存池结构
 */
static void mempool_t_init(mempool_t *pool){
	spinlock_init(&pool->lock);
	list_t_init(&pool->hook);
	pool->status = 0;
	pool->flags = 0;
	
	for(uint_t i=0;i<MEMPOOL_LIST_MAX;i++){
		list_t_init(&pool->pages[i]);		
	}
	return;
}

/**
 * 将占用的页挂载到内存池
 */
static void mount_mempage(mempool_t* pool,MemPage* page,size_t size){
	//获取size关于2的阶
	sint_t order = get_order(size);

	cpuflag_t cpuflag;
	spinlock_lock_cli(&pool->lock,&cpuflag);
	
	//将分配的内存页挂载到指定位置的链表处
	list_add(&page->hook,&pool->pages[order]);

	spinlock_unlock_sti(&pool->lock,&cpuflag);
	return;
}



/**
 * 从内存池移除占用的页
 */
static MemPage* remove_mempage(mempool_t* pool,addr_t adr,size_t num){
	//获取size关于2的阶
	sint_t order = get_order(num);
	
	cpuflag_t cpuflag;
	spinlock_lock_cli(&pool->lock,&cpuflag);

	list_t *pos = NULL;
	MemPage* page = NULL;
	list_for_each(pos,&pool->pages[order]){
		page = list_entry(pos,MemPage,hook);
		//判断page是否是地址adr对应的page
		if(adr==mempage_vadr(page)){
			//脱链
			list_del(&page->hook);
			goto out;		
		}
	}
	page = NULL;
out:
	spinlock_unlock_sti(&pool->lock,&cpuflag);
	return page;
}


/**
 * 内存对象分配
 * return:内存页虚拟地址
 */
static addr_t new_memobject(size_t size){
	return (addr_t)mobj_new(size);
}

/**
 * 内存对象释放
 */
static bool_t delete_memobject(addr_t adr,size_t size){
	return mobj_delete((void*)adr,size);
}

/**
 * 内存页分配
 * return：内存页虚拟地址
 */
static addr_t new_mempage(size_t size){
	//获取size大小内存所需的页数
	size_t count = ALIGN_4K(size)>>12;

	//从内核区分配内存页
	size_t realNumber = 0;
	MemPage* page = mem_divide(&memmgr,count,&realNumber,MEMAREA_TYPE_KRNL,MEMTABLE_RELDIV);

	if(NULL==page){
		//内存分配失败
		return NULL;
	}

	//将分配的内存页挂载到内存池中
	mount_mempage(&mempool,page,realNumber);

	//返回的必须是虚拟地址
	return mempage_vadr(page);
}
/**
 * 内存页释放
 */
static bool_t delete_mempage(addr_t adr,size_t size){
	//获取size大小内存所需的页数
	size_t count = ALIGN_4K(size)>>12;
	
	//从内存池中获取已分配待释放的mempage
	MemPage* page = remove_mempage(&mempool,adr,count);

	if(NULL==page){
		//释放地址所在页并未挂载在内存池
		return NULL;
	}

	//将内存页归还给物理内存管理器
	return mem_merge(&memmgr,page,count);
}

/**
 * 内存分配核心函数
 */
static addr_t mempool_new_core(size_t size){
	//根据申请内存大小决定是按照内存对象分配还是按页分配
	if(size<=MEMPOOL_LIMIT){
		//分配内存对象
		return new_memobject(size);
	}else{
		//分配内存页
		return new_mempage(size);	
	}
}

/**
 * 内存释放核心函数
 */
static bool_t mempool_delete_core(addr_t adr,size_t size){
	//根据申请内存大小决定是删除内存对象还是删除内存页
	if(size<=MEMPOOL_LIMIT){
		//删除的是内存对象
		return delete_memobject(adr,size);
	}else{
		//删除的是内存页
		return delete_mempage(adr,size);
	}
}


addr_t mempool_new(size_t size){
	//参数检查
	if(size<MEMPOOL_APPLY_MIN || size>MEMPOOL_APPLY_MAX){
		return NULL;
	}
	//调用核心分配函数分配内存
	return mempool_new_core(size);
}


bool_t mempool_delete(addr_t adr,size_t size){
	//参数检查
	if(NULL==adr || size<MEMPOOL_APPLY_MIN || size>MEMPOOL_APPLY_MAX){
		return FALSE;
	}
	//调用释放核心函数
	return mempool_delete_core(adr,size);
}


void mempool_init(){
	//初始化内存池管理结构体
	mempool_t_init(&mempool);

	//测试提供的API
	mempool_test_main();
	return;
}


//==============================================================================================
INLINE void test_mempool_new();
INLINE void test_mempool_delete();

void mempool_test_main(){
	//test_mempool_new();
	//test_mempool_delete();
	
	//die(0);
	return;
}

INLINE void test_mempool_new(){
	/*
	//[1,2048]
	for(size_t i=1;i<=2048;i++){
		printk("0x%lx\n",i);
		if(mempool_new(i)==NULL){
			printk("error in mempool_new:%d\n",i);
			die(0);
		}
	}
	*/
	/*
	//(2048,4096]
	for(size_t i=2048+1;i<=4096;i++){
		printk("0x%lx\n",i);
		if(mempool_new(i)==NULL){
			printk("error in mempool_new:%d\n",i);
			die(0);
		}	
	}
	*/
	//[4096,4MB]
	for(size_t i=0x380000;i<=0x400000;i+=0x1000){
		printk("0x%lx\n",i);
		if(mempool_new(i)==NULL){
			printk("error in mempool_new:0x%lx\n",i);
			die(0);
		}	
	}
	
	
	//size<=0
	printk("0x%lx\n",0);
	if(mempool_new(0)!=NULL){
		printk("error in mempool_new:0\n");
	}
	printk("0x%lx\n",0x400000+1);
	if(mempool_new(0x400000+1)!=NULL){
		printk("error in mempool_new:4MB+1\n");
	}
	printk("0x%lx\n",0x400000+2);
	if(mempool_new(0x400000+2)!=NULL){
		printk("error in mempool_new:4MB+2\n");
	}
	printk("0x%lx\n",0x400000+3);
	if(mempool_new(0x400000+3)!=NULL){
		printk("error in mempool_new:4MB+3\n");
	}

	printk("successfully pass mempool_new test\n");
	return;
}

INLINE void test_mempool_delete(){
	addr_t adr = NULL;
/*
	for(size_t i=1;i<=2048;i++){
		adr = mempool_new(i);
		if(NULL==adr){
			printk("error in mempool_new:%d\n",i);
			die(0);
		}

		if(mempool_delete(adr,i)==FALSE){
			printk("error in mempool_delete:%d\n",i);
			die(0);
		}
	}
*/
	//[4096,4MB]
	for(size_t i=0x380000;i<=0x400000;i+=0x1000){
		adr = mempool_new(i);
		if(NULL==adr){
			printk("error in mempool_new:%d\n",i);
			die(0);
		}
		if(mempool_delete(adr,i)==FALSE){
			printk("error in mempool_delete:%d\n",i);
		}
	}

	printk("successfully pass mempool_delete test\n");
	
	return;
}
