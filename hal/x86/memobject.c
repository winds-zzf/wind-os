/**
 * 内存对象管理
 * API实现
 */

#include "globaltype.h"
#include "globalctrl.h"

static void init_mObjManager(MObjManager *manager);

/**
 * 内存对象的初始化只需要初始化管理器和挂载点
 */
void memobject_init(){
	//初始化内存对象管理器
	init_mObjManager(&memmgr.objManager);
	
	return;
}


/**
 * 简单初始化内存对象容器挂载点
 */
static void init_mObjMount(MObjMount *mount,size_t size){
	list_init(&mount->containers);
	//挂载点初始并不挂载容器，容器是申请时动态分配的
	mount->containers_num = 0;
	//挂载点缓存有近期使用的容器，一个挂载点可以同时有多个容器，但是他们对象大小相同
	mount->cache = NULL;
	mount->size = size;		//设置挂载点内存对象的大小
	
	return;
}

/**
 * 初始化内存对象管理，并设置64个挂载点
 */
static void init_mObjManager(MObjManager *manager){
	spinlock_init(&manager->lock);
	
	//初始化挂载点数组
	size_t size = 0;
	for(uint_t i=0;i<MOBJMOUNT_MAX;i++){
		//内存对象大小依次为32,64,96,...,2048共64个
		size+=32;
		//依次初始化MOBJMOUNT个挂载点，并依次设置他们的内存对象大小
		init_mObjMount(&manager->mounts[i],size);
	}
	
	manager->number = 0;
	
	return;
}

//---------------------------------------------------------------------------------------------------------

/**
 * 初始化内存对象
 */
static void init_mObject(MObject *object,uint_t status){
	list_init(&object->hook);
	object->status = status;
	object->start = object;	//将MObject结构体暂存在未分配对象开始地址处(充分利用空闲内存空间)
	
	return ;
}

/**
 * 简单初始化内存对象容器拓展
 */
static void init_mObjExtend(MObjExtend *extend,addr_t start,addr_t end,MObjContainer *container){
	list_init(&extend->hook);
	extend->start_addr = start;
	extend->end_addr = end;
	extend->container = container;	//拓展空间所属的容器
	
	return;
}

/**
 * 简单初始化内存对象容器
 */
static void init_mObjContainer(MObjContainer *container,addr_t start,addr_t end,size_t size){
	spinlock_init(&container->lock);
	list_init(&container->hook);
	container->status = 0;
	container->flag = 0;
	//容器起止地址，对象大小
	container->start_addr = start;
	container->end_addr = end;
	container->mobj_size = size;
	
	container->total_num = 0;
	list_init(&container->assigns);
	list_init(&container->frees);
	container->frees_num = 0;
	list_init(&container->extends);
	container->extends_num = 0;
	
	//管理分配的物理内存页表
	list_init(&container->mems.basic);		//容器基本内存页
	list_init(&container->mems.extends);	//容器拓展内存页
	container->mems.size = 0;			//连续内存页数量
	
	return;
}

//---------------------------------------------------------------------------------------------------------
/**
 * 检查这个内存对象容器是否能够满足条件，经常使用的代码，直接使用INLINE内嵌
 */
INLINE bool_t check_container(MObjContainer *container,size_t size){
	//只要内存对象大小不小于size即可
	return size <= container->mobj_size? TRUE:FALSE;
}


/**
 * 从挂载点中找到一个合适的容器
 * 先查看cache，cache不命中才去遍历
 */
static MObjContainer* find_container_for_create(MObjMount *mount){
	//先检查挂载点的容器缓存
	if(mount->cache){
		printk("hit:");
		return mount->cache;	//cache命中
	}
	printk("miss:");
	//cache未命中，进一步检查挂载点是否挂载有容器
	if(mount->containers_num>0){
		List *pos=NULL;
		//遍历挂载点挂载的所有容器
		list_for_each(pos,&mount->containers){
			//首次链表为空，会导致cache=NULL
			mount->cache = list_entry(pos,MObjContainer,hook);
			return mount->cache;
		}
	}
	
	return NULL;
}


/**
 * 把内存对象容器挂载到对应挂载点
 */
static bool_t mount_container(MObjMount *mount,MObjContainer *container){
	//将容器挂载到挂载点
	list_add(&container->hook,&mount->containers);
	mount->containers_num++;	//容器计数加一
	
	return TRUE;
}


/**
 * 初始化内存对象容器
 */
static MObjContainer* _create_container(addr_t start,addr_t end,size_t size,MemPage* pages,size_t pageNumber){
	//将开头的内存区域初始化为容器数据
	MObjContainer *container = (MObjContainer*)start;	
	//初始化容器
	init_mObjContainer(container,start,end,size);

	//把subpage挂载到内存页管理字段
	list_add(&pages->hook,&container->mems.basic);
	container->mems.size = pageNumber;	//记录这个容器每个连续内存页面的大小
	
	//紧接着Container开始存放并初始化内存对象数组
	addr_t adr = (addr_t)(container+1);	//获取内存对象数组起始地址
	MObject *obj = NULL;
	for(; adr+size<=end+1; adr+=size){		//注意end=start+size-1
		obj = (MObject*)adr;
		//借用内存对象开头部分空间存储内存对象结构体
		init_mObject(obj,0);
		//把MObject对象挂载到容器的空闲对象空间
		list_add(&obj->hook,&container->frees);
		container->frees_num++;
		container->total_num++;
	}
	printk("size:%d\n",container->frees_num);
	return container;
}


/**
 * 首次分配内存对象的时候，需要创建内存对象容器
 * size：这里size是容器内存对象大小，不是请求的大小
 */
static MObjContainer* create_container(MObjManager *manager,MObjMount *mount){
	//确定容器需要多少个页面
	size_t pageNumber;	//默认值
	if(mount->size<=128){
		pageNumber = 1;
	}else if(mount->size<=512){
		pageNumber = 2;
	}else {
		pageNumber = 4;
	}
	
	//为容器分配物理内存空间，也就是之前实现的物理内存页面管理器
	size_t realNumber=0;
	MemPage *pages = mem_divide(&memmgr,pageNumber,&realNumber,MEMAREA_TYPE_KRNL,DMF_RELDIV);
	if(NULL==pages){	
		return NULL;	//内存分配失败
	}

	//获取连续物理页面起止物理地址
	addr_t startAddr = pages->addr.value<<12;
	addr_t endAddr = (addr_t)startAddr+(realNumber<<12)-1;
	//将起止地址转换为虚拟地址
	startAddr = MAP_RV(startAddr);
	endAddr = MAP_RV(endAddr);

	//初始化内存对象容器
	MObjContainer *container = _create_container(startAddr,endAddr,mount->size,pages,realNumber);

	//将容器挂载到挂载点
	mount_container(mount,container);
	
	//容器计数+1
	manager->number++;
	
	return container;
}

/**
 * 初始化容器拓展
 * start&end must be virtual address
 */
static bool_t _extend_container(MObjContainer *container,addr_t start,addr_t end,size_t size,MemPage*pages){
	//1.将内存页其实区域分配给MObjExtend结构
	MObjExtend *extend = (MObjExtend*)start;
	
	//2.在内存区域头部建立MObjExtend结构
	init_mObjExtend(extend,start,end,container);
	
	//3.循环建立拓展区域的内存对象
	addr_t adr = start+sizeof(MObjExtend);
	MObject *obj = NULL;
	for(; (adr+size)<=end+1; adr+=size){
		obj = (MObject*)adr;
		//初始化内存对象
		init_mObject(obj,0);
		//将内存对象挂载到空闲链表
		list_add(&obj->hook,&container->frees);
		container->frees_num++;
		container->total_num++;
	}
	
	//4.将容器拓展MObjExtend挂载到拓展链表
	list_add(&extend->hook,&container->extends);
	container->extends_num++;	//拓展页面数+1
	
	//5.将所需内存页面挂载到对应页面
	list_add(&pages->hook,&container->mems.extends);
	
	return TRUE;
}

/**
 * 对容器Container进行拓展
 */
static bool_t extend_container(MObjContainer *container){
	//确定容器需要多少个页面
	size_t pageNumber;	//默认值
	if(container->mobj_size<=128){
		pageNumber = 1;	//[0,128]
	}else if(container->mobj_size<=512){
		pageNumber = 2;	//[129,512]
	}else {
		pageNumber = 4;	//[512,2048]
	}

	//向物理内存管理器从内核地址空间分配pageNumber大小的连续页面
	size_t realNumber = 0;
	MemPage *pages = mem_divide(&memmgr,pageNumber,&realNumber,MEMAREA_TYPE_KRNL,DMF_RELDIV);
	if(NULL==pages){
		//内存分配失败
		return FALSE;
	}

	//获取分配连续物理页面的起止地址	(可以转换成MemTable的API)
	addr_t startAddr = pages->addr.value<<12;
	addr_t endAddr = (addr_t)startAddr + (realNumber<<12) - 1;	//分配的是连续的内存页面
	//将物理页面起止地址转换为程序可用的虚拟地址
	startAddr = MAP_RV(startAddr);
	endAddr = MAP_RV(endAddr);

	//将分配的内存空间初始化为容器拓展
	if(!_extend_container(container,startAddr,endAddr,container->mobj_size,pages)){
		return FALSE;
	}

	return TRUE;
}


/**
 * 实际分配内存对象
 */
static void* _allocate_object(MObjContainer *container){
	//从容器中取出第一个空闲内存对象(next:首元结点)
	//无论内存对象有多少，获取内存对象的操作始终是固定的，因此较为高效
	MObject *object = list_entry(container->frees.next,MObject,hook);
	
	//从链表中脱链
	list_del(&object->hook);
	
	//容器中空闲对象数-1
	container->frees_num--;
	
	//返回内存对象首地址
	return (void*)object;
}

/**
 * 
 */
static void* allocate_object(MObjContainer * container){
	void *object = NULL; 
	cpuflag_t cpuflag = 0;
	//将container上锁并关中断
	spinlock_lock_cli(&container->lock,&cpuflag);
	
	//如果容器中没有空闲对象了，就需要拓展容器内存了
	if(container->frees_num < 1){
		//拓展内存对象容器
		if(!extend_container(container)){
			object = NULL;
			goto ret_step;
		}
	}
	//实际分配内存对象
	object = _allocate_object(container);

ret_step:
	//容器解锁并开中断
	spinlock_unlock_sti(&container->lock,&cpuflag);
	return object;
}


/**
 * 内存对象分配核心函数
 * size: 请求分配的的内存对象大小
 */
static void* mobj_create_core(size_t size){
	//获取内存对象管理器指针
	MObjManager *manager = &memmgr.objManager;
	void* object =NULL;	 //查找到的内存对象

	//内存对象管理器上锁并关中断
	cpuflag_t cpuflag = 0;
	spinlock_lock_cli(&manager->lock,&cpuflag);

	//1.根据申请大小size在manager中查找容器挂载点
	MObjMount *mount = &manager->mounts[(size-1)/32];
	
	
	//2.从挂载点中查找合适的容器（一个挂载点可能有多个容器）
	MObjContainer *container = find_container_for_create(mount);
	if(NULL==container){
		//如果没有找到容器就尝试新建一个容器
		container = create_container(manager,mount);
		if(NULL==container){
			//新建容器失败
			object = NULL;
			goto ret_step;	
		}
	}
	
	//3.从容器中分配一个内存对象，更新cache
	object = allocate_object(container);
	mount->cache = container;
	if(NULL==object){
		object = NULL;
		goto ret_step;
	}
	
ret_step:
	//内存对象管理器解锁并开中断
	spinlock_unlock_sti(&manager->lock,&cpuflag);
	return object;
}

/**
 * 内存对象分配接口函数（接口处一般进行参数检查）
 * size: 请求分配的内存对象大小
 * 类似于calloc，动态分配的小内存返回的是无类型指针，实际使用时强转成具体类型
 */
void* mobj_new(size_t size){
	//参数检查
	if(0==size || size>2048){
		return NULL;
	}

	//调用核心函数进行内存对象分配
	return mobj_create_core(size);
}

/**
 * 检查要释放的内存对象的地址是否属于该容器
 */
static bool_t check_container_for_delete(MObjContainer *container,void *start,size_t size){
	//检查待释放对象是否落在容器基础空间中(注意两端都是可以取等的)
	if((addr_t)start>=container->start_addr+sizeof(MObjContainer)&&((addr_t)start+size)<=container->end_addr){
		//检查释放的内存对象大小是否不超过本容器支持的对象大小
		if(size<=container->mobj_size){
			return TRUE;
		}
	}	

	//未落在基础容器空间，依次在每个拓展地址空间中查找
	if(container->extends_num<1){	
		//判断是否存在拓展空间，没有的话直接返回
		return FALSE;
	}
	
	//检查每个拓展空间
	MObjExtend* extand = NULL;
	List *pos = NULL;
	list_for_each(pos,&container->extends){
		extand = list_entry(pos,MObjExtend,hook);	//获取一个拓展空间对象
		if((addr_t)start>=extand->start_addr+sizeof(MObjExtend)&&((addr_t)start+size-1)<=container->end_addr){
			if(size<=container->mobj_size){
				return TRUE;	
			}
		}
	}
	return FALSE;
}

/**
 * 
 */
static MObjContainer* find_container_for_delete(MObjMount *mount,void *start,size_t size){
	//查看cache缓存
	if(mount->cache && check_container_for_delete(mount->cache,start,size)){
		//缓存命中,直接返回对象
		return mount->cache;
	}	
	//缓存未命中，遍历挂载点上挂载的所有容器
	if(mount->containers_num>0){	//挂载点上至少一个容器
		List *pos = NULL;
		MObjContainer *container = NULL;
		list_for_each(pos,&mount->containers){
			//取出一个容器
			container = list_entry(pos,MObjContainer,hook);
			//检查待释放内存对象是否在该容器中
			if(check_container_for_delete(container,start,size)){
				return container;
			}
		}
	}
	return NULL;
}


/**
 * 从容器中回收内存对象核心
 */
static bool_t _recycle_mObject(MObjContainer *container,void *start,size_t size){
	//检查释放是否合法，如果free_num+1>total_num,说明存在重复释放
	if((1+container->frees_num)>container->total_num){
		return FALSE;
	}
	
	/**
	 * 在回收内存对象的时候，无需考虑内存对象是在容器基础空间中还是在拓展空间中，只需要知道内存对象的首地址，
	 * 将MObject初始化进去，然后再将内存对象挂在到free_list即可
	 */
	MObject *object = (MObject*)start;
	//借用空闲对象存储MObject结构 
	init_mObject(object,0);
	//将对象挂载到空闲链表上
	list_add(&object->hook,&container->frees);
	//容器空闲对象数+1
	container->frees_num++;
	
	return TRUE;
}


/**
 * 从容器中回收对象框架
 */
static bool_t recycle_mObject(MObjContainer *container,void *start,size_t size){
	bool_t rets = FALSE;
	//容器加锁并关中断
	cpuflag_t cpuflag = 0;
	spinlock_lock_cli(&container->lock,&cpuflag);

	//实现完成内存对象的释放
	if(!_recycle_mObject(container,start,size)){
		rets = FALSE;
		goto ret_step;
	}
	rets = TRUE;
ret_step:
	//容器解锁并开中断
	spinlock_unlock_sti(&container->lock,&cpuflag);
	return rets;
}


/**
 * 检查一个容器是否为空
 * 如果总共total_num个内存对象都是空闲的，那么这个容器就可以被销毁了
 */
static bool_t destroy_check(MObjContainer *container){
	return container->frees_num == container->total_num;
}


/**
 * 当内存容器中的所有对象都释放时，销毁容器
 */
static bool_t _destroy_container(MObjManager *manager,MObjMount *mount,MObjContainer *container){
	//将容器从挂载点脱离,挂载点容器计数-1,如果可能，那么修改缓存
	list_del(&container->hook);
	mount->containers_num--;
	//如果待删除容器被缓存，那么清空缓存
	if(mount->cache==container) mount->cache=NULL;
	//总容器数减1
	manager->number--;
	
	//释放容器拓展页面
	List *pos = NULL;
	MemPage *pages = NULL;
	if(container->extends_num>0){
		//依次遍历每个subpages
		list_for_each_head_dell(pos,&container->mems.extends){
			pages = list_entry(pos,MemPage,hook);
			//将subpage脱链
			list_del(&pages->hook);
			//释放这段内存页面
			printk("reback subpage\n");
			if(!mem_merge(&memmgr,pages,container->mems.size)){
				printk("error\n");
			}
		}
	}

	//释放容器本身占用的物理页面
	list_for_each_head_dell(pos,&container->mems.basic){
		pages = list_entry(pos,MemPage,hook);
		list_del(&pages->hook);
		if(!mem_merge(&memmgr,pages,container->mems.size)){
			printk("error\n");
		}
	}	
	
	return TRUE;
}


/**
 * 当内存容器中的所有对象都释放时，销毁容器
 */
static bool_t destroy_container(MObjManager *manager,MObjMount *mount,MObjContainer *container){
	//看看可不可以销毁
	if(destroy_check(container)){
		//执行销毁任务
		return _destroy_container(manager,mount,container);
	}

	return FALSE;
}

/**
 * 内存释放核心函数
 */
static bool_t mobj_recycle_core(void* addr,size_t size){
	//获取内存对象管理器指针
	MObjManager *manager = &memmgr.objManager;
	bool_t rets = FALSE;

	//内存对象管理器加锁并关中断
	cpuflag_t cpuflag = 0;
	spinlock_lock_cli(&manager->lock,&cpuflag);

	//根据释放内存对象的大小，在MObjManager中找到合适的挂载点MObjMount
	MObjMount *mount = &manager->mounts[(size-1)/32];
	if(NULL==mount){
		rets = FALSE;
		goto ret_step;
	}

	//在挂载点中找到对象所在容器
	MObjContainer *container = find_container_for_delete(mount,addr,size);
	if(NULL==container){
		rets = FALSE;
		goto ret_step;
	}

	//从所在容器中回收内存对象
	if(!recycle_mObject(container,addr,size)){
		rets = FALSE;
		goto ret_step;
	}
	
	//尝试销毁容器（如果容器中的内存对象都已经被回收，则销毁容器）
	destroy_container(manager,mount,container);
	
	//如果执行到这一步，说明前面的所有步骤都成功执行了
	rets = TRUE;
//公共处理
ret_step:
	//内存对象管理器解锁并开中断
	spinlock_unlock_sti(&manager->lock,&cpuflag);

	return rets;
}


/**
 * 内存对象回收函数接口
 * addr: 待释放内存对象虚拟地址
 * size: 待释放内存对象大小(unsigned integer非负数)
 */
bool_t mobj_delete(void *addr,size_t size){
	if(NULL==addr || size>2048){
		return FALSE;
	}
	
	//调用内存对象释放核心函数
	return mobj_recycle_core(addr,size);
}


//==============================================================================================
INLINE void test_mobj_new();		//综合测试
INLINE void test_mobj_delete();	//综合测试
INLINE void test_create_container();	
INLINE void test_extend_container();
INLINE void test_delete_container();
INLINE void test_destroy_container();
INLINE void test_find_container_for_delete();
INLINE void test_find_container_for_create();
INLINE void test_hit();	//cache命中测试

void memobject_test_main(){
	printk("testing the memobject\n");
	//test_mobj_new();		//综合测试
	//test_mobj_delete();	//综合测试
	//test_create_container();	
	//test_extend_container();
	test_delete_container();
	//test_destroy_container();
	//test_find_container_for_delete();
	//test_find_container_for_create();

	die(0);
	return;
}

INLINE void test_mobj_new(){
	MObject* obj  = NULL; 
	
	obj = (MObject*)mobj_new(123);
	printk("object:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(132);
	printk("object:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(258);
	printk("object:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(513);
	printk("object:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(1024);
	printk("object:0x%lx,0x%lx\n",obj->status,obj->start);	
	return;
}

INLINE void test_mobj_delete(){
	
	
	return;
}

INLINE void test_create_container(){
	MObject *obj  = NULL; 

	//越界访问
	obj = (MObject*)mobj_new(0);
	if(!obj); else printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(2049);
	if(!obj); else printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);

	//正常测试
	obj = (MObject*)mobj_new(1);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(29);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(30);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);	
	obj = (MObject*)mobj_new(31);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(32);
	
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(33);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);

	obj = (MObject*)mobj_new(127);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(128);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(129);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);

	obj = (MObject*)mobj_new(511);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(512);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(513);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);

	obj = (MObject*)mobj_new(2047);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);
	obj = (MObject*)mobj_new(2048);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);
	
	return;
}

INLINE void test_extend_container(){
	MObject *obj = NULL;

	obj = (MObject*)mobj_new(1);
	printk("obj:0x%lx,0x%lx\n",obj->status,obj->start);
	for(uint_t i=0;i<121;i++){
		mobj_new(23);
	}
	printk("=========\n");
	mobj_new(23);
	mobj_new(23);
	mobj_new(23);
	mobj_new(23);
	return;
}

INLINE void test_delete_container(){
	//申请内存对象
	/*
	printk("%lx,%lx\n",sizeof(MObjContainer),sizeof(MObjMount));
	MObject *obj1 = (MObject*)mobj_new(16);
	printk("obj:0x%lx,0x%lx\n",obj1->status,obj1->start);
	MObject *obj2 = (MObject*)mobj_new(16);
	printk("obj:0x%lx,0x%lx\n",obj2->status,obj2->start);
	MObject *obj3 = (MObject*)mobj_new(16);
	printk("obj:0x%lx,0x%lx\n",obj3->status,obj3->start);	
	*/
	void* arrays[122];
	//将一个容器容量分配完
	for(uint_t i=0;i<=121;i++){
		arrays[i] = (MObject*)mobj_new(16);
		if(!arrays[i]){
			printk("error1:%d\n",i);
			die(0);
		}
	}
	//测试拓展容量
	MObject *obj = (MObject*)mobj_new(16);

	mobj_delete(obj,32);
	for(uint_t i=0;i<=121;i++){
		if(!mobj_delete(arrays[i],30)){
			printk("error2:%d\n",i);
			die(0);
		}
	}
	
	return;
}

INLINE void test_destroy_container(){
	
	return;
}

INLINE void test_find_container_for_create(){
	
	return;
}

INLINE void test_find_container_for_delete(){
	
	return;
}

INLINE void test_hit(){
	return;
}
