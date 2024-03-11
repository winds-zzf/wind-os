/**
 * memory mananer interface
 * API's implementations
 */

#include "globaltype.h"
#include "globalctrl.h"


/**
 * get the MemArea from mgr by areaType
 */
static MemArea* get_memArea(memmgr_t *mgr,uint_t areaType){
	//parameters testing
	if(NULL==mgr || areaType>=mgr->areas_num || areaType<1){
		return NULL;
	}
	else return &mgr->areas[areaType-1];
}


sint_t get_order(size_t size){
	if(0==size) return -1;
	//
	for(sint_t offset=0; offset<64; offset++){
		if(0x1ULL<<offset >= size){
			return offset;
		}
	}
	return -1;
}


/**
 * 从MemList中获取subpage子串（首元节点）,并修改相应的统计信息，
 * 通过参数返回subpage头节点，并根据返回值（长度）判断是否获取成功
 */
static bool_t get_subpages(MemList *mlist,MemPage **head,MemPage **tail){
	//获取挂在首元结点上的元素
	MemPage *subpage = list_entry(mlist->free_pages.next,MemPage,hook);
	
	//从链表中删除首元结点
	list_del(&subpage->hook);
	
	//空闲链表长和总长度-1
	mlist->free_num--;
	mlist->total_num--;

	//返回subpage(subpage长度为1时也成立)
	*head = subpage;	//subpage头节点
	*tail = &subpage[mlist->size-1];	//subpage尾节点
	
	return TRUE;	//范围subpage长度
}


/**
 * 将连续subpage(head,tail)插入到List链表下
 */
static bool_t put_subpages(MemList *mlist, MemPage *head,MemPage* tail){
	//更新子串
	head->flags.olkty = PAGEFLAGS_OLKTY_ODER;
	head->next = tail;
	/**
	 * 为什么加上这一句会导致计数出错
	 */
	//tail->flags.olkty = PAGEFLAGS_OLKTY_BAFH;
	//tail->next = mlist;
	//将子串添加到链表
	list_add(&head->hook,&mlist->free_pages);
	//更新统计信息
	mlist->free_num++;
	mlist->total_num++;
	return TRUE;
}

//以上部分将会被整合到memTable模块
//---------------------------------------------------------------------------------------------

/**
 * 分割内存所需要list范围
 * 分割内存时从高阶链表到低阶链表
 */
static bool_t divide_list_range(MemArea *area,size_t size,MemList **startList,MemList **endList){
	MemList* lists = area->table.lists;

	//获取size关于2的阶数
	sint_t order = get_order(size);
	if(order<0){ //size=0或者order过大
		return FALSE;
	}
	
	//虽然size关于2的阶是order，但是order处对应MemList链表可能为空，所以需要依次往后找
	for(sint_t i = order; i<MEMLIST_MAX; i++){
		if(lists[i].free_num>0){
			*startList = &lists[i];
			*endList = &lists[order];
			return TRUE;
		}
	}

	*startList = NULL;
	*endList = NULL;
	return FALSE;
}


/**
 * 合并内存所需要的list范围
 * 合并内存是从低阶链表到高阶链表
 */
static bool_t merge_list_range(MemArea *area,size_t size,MemList **startList,MemList **endList){
	MemList* lists = area->table.lists;
	
	//获取size关于2的阶,并检查
	sint_t order = get_order(size);
	if(order<0 || order>MEMLIST_MAX){
		*startList = NULL;
		*endList = NULL;
		return FALSE;
	}

	*startList = &lists[order];
	*endList = &lists[MEMLIST_MAX-1];
	
	return TRUE;
}


/**
 * 更新取出的子串信息 
 * 1.修改页面标志位
 * 2.修改指针
 */
static MemPage* divide_update_subpages(MemPage *head,MemPage *tail){
	//单页subpage
	if(head==tail){
		head->flags.count++;	//页面分配计数+1，因为可能是多个页面在共享使用同一个页
		head->addr.allocate = PAGEADDR_ALLOC;	//物理地址标记为已分配
		head->flags.olkty = PAGEFLAGS_OLKTY_ODER;	//页面挂入链表类型
		head->next = tail;
	}else{//多页subpage
		head->flags.olkty = PAGEFLAGS_OLKTY_ODER;
		head->flags.count++;
		head->addr.allocate = PAGEADDR_ALLOC;
		//多个MemPage的情况下，末端的结构也设置为已分配状态
		tail->flags.count++;
		tail->addr.allocate = PAGEADDR_ALLOC;
		//头尾相连
		head->next = tail;
	}

	return head;
}


/**
 * 
 */
static MemPage* divide_execute(MemArea *area,size_t *realSize,MemList *startList,MemList *endList){
	size_t retSize = 0;
	MemPage *retPage = NULL;
	MemPage *startPage=NULL,*endPage = NULL;

	//从高阶的MemList中取出一个subpage并拆分合并
	get_subpages(startList,&startPage,&endPage);
	for(MemList *cur=startList-1; cur>=endList; cur--){
		//将从startList中取下来的subpage连续拆分到低阶MemList中
		put_subpages(cur,&startPage[cur->size],endPage);
		endPage -= cur->size;
	}
	
	retSize = endList->size;
	//修改获取的subpage的标志位，表示已为删除
	retPage = divide_update_subpages(startPage,endPage);
	
	//result testing
	if(NULL==retPage){
		*realSize = 0;
		return NULL;
	} else {
		*realSize = retSize;
		return retPage;
	}
}


/**
 * the core function of memory division
 */
static MemPage* divide_core(MemArea *area,size_t requestSize,size_t *realSize,uint_t flags){
	//获取目标MemList和最大MemTarget
	MemList *startList = NULL,*endList = NULL;
	//只适用于连续内存分割，而不是用于应用区内存页面的分割
	if(!divide_list_range(area,requestSize,&startList,&endList)){
		*realSize = 0;
		return NULL;
	}

	//调用divide_execute执行分割任务
	size_t retSize = 0;
	MemPage *retPage = divide_execute(area,&retSize,startList,endList);
	
	//result testing
	if(NULL==retPage){
		*realSize = 0;
		return NULL;
	}
	else{
		*realSize = retSize;
		return retPage;	
	}
}

/**
 * the frame function of memory division
 * area:
 * requestSize：
 * realSize: 
 * flags: 
 * return:
 */
static MemPage* divide_frame(memmgr_t *mgr,size_t requestSize,size_t *realSize,uint_t areaType,uint_t flags){
	size_t retSize = 0;
	MemPage *retPage = NULL;

	//1.get the target memory area
	MemArea *area = get_memArea(mgr,areaType);
	if(NULL==area){	//error memory area
		*realSize = 0;
		return NULL;
	}
	
	cpuflag_t cpuflag = 0;
	//2.lock the memory area and close interrupt
	spinlock_lock_cli(&area->lock,&cpuflag);
	
	//
	if(MEMTABLE_RELDIV==flags){
		//execute memory allocating
		retPage = divide_core(area,requestSize,&retSize,flags);
	}
	
	//3.unlock the memory area and start interrupt
	spinlock_unlock_sti(&area->lock,&cpuflag);	

	//4.result testing
	if(NULL==retPage){
		*realSize = 0;
		return NULL;
	}
	else {
		*realSize = retSize;
		return retPage;
	}
}


/**
 * the interface function of memory division
 * mgr: Memmgr struct
 * requestSize: the number of pages what applied for
 * realSize: the number of pages what are actually allocated
 * areaType: the area type of pages requested
 * flags: page flags
 * return：the continued MemPages allocated from specified area by specified size 
 */
MemPage* mem_divide(memmgr_t *mgr,size_t requestSize,size_t *realSize,uint_t areaType,uint_t flags){
	//parameters testing
	if(NULL==mgr|| 0==requestSize ||NULL==realSize||0==areaType){	//error parameters
		return NULL;
	}
	
	//invoking the frame function
	size_t retSize = 0;
	MemPage *retPage = divide_frame(mgr,requestSize,&retSize,areaType,flags); 
	
	//result testing
	if(NULL==retPage){
		*realSize = 0;
		return NULL;
	}
	*realSize = retSize;
	return retPage;
}

/**
 * 
 */
static MemPage* mem_divide_apps_core(memmgr_t *mgr){
	MemPage *rets = NULL;
	//获取内存应用区
	MemArea *area = get_memArea(mgr,MEMAREA_TYPE_APPS);
	if(NULL==area){
		//内存区获取失败
		return NULL;
	}

	cpuflag_t cpuflag;
	spinlock_lock_cli(&mgr->lock,&cpuflag);
	//检查内存区是否够分配
	if(area->free_num<1){
		rets = NULL;
		goto ret_step;
	}

	{//执行内存分配
		MemList *list = &area->table.list;		//获取懒分配链表
		rets = list_entry(list->free_pages.next,MemPage,hook);	//获取首元结点用于分配
		list_del(&rets->hook);
		//更新信息
		list->free_num--;		//空闲数-1
	}

	//如果分配成功，更新信息
	area->free_num--;
	area->allocate_num++;

ret_step:
	spinlock_unlock_sti(&mgr->lock,&cpuflag);
	return rets;
}



/**
 * 应用区内存分配
 */
MemPage* mem_divide_apps(memmgr_t *mgr){
	//参数检查
	if(NULL==mgr){
		return NULL;
	}
	
	//尝试懒分配
	MemPage *page = mem_divide_apps_core(mgr);
	if(NULL!=page){	//分配成功
		return page;
	}
	
	//懒分配失败，尝试从内核区分配内存页
	size_t realNumber = 0;
	page = mem_divide(mgr,1,&realNumber,MEMAREA_TYPE_KRNL,MEMTABLE_RELDIV);

	//返回分配的内存页(包括NULL的情况)
	return page;
}


/**
 * 更新subpages的信息，如果需要进一步的合并，返回true，否则返回false
 * 虽然指示了挂载位置，但是并没有真正加入到MemList中
 */
static bool_t merge_update_subpages(MemList *mlist,MemPage *head,size_t size){
	MemPage *tail = head+size-1;
	if(head==tail){	//处理只有单页的情况
		//页面分配计数-1
		head->flags.count--;
		if(head->flags.count>0){	//页面仍被共享，不可进行合并
			return FALSE;
		}
		//页面不再被任何进程使用，需要合并回收
		head->addr.allocate = PAGEADDR_NO_ALLOC;	//设置地址未分配位
		head->flags.olkty = PAGEFLAGS_OLKTY_BAFH;	//挂载链表类型为BAFH
		head->next = mlist;
		return TRUE;
	}else{	//处理多页的情况
		head->flags.count--;
		tail->flags.count--;
		if(head->flags.count>0){
			return FALSE;
		}
		//修改地址分配位和连接
		head->addr.allocate = PAGEADDR_NO_ALLOC;
		head->flags.olkty = PAGEFLAGS_OLKTY_ODER; 
		head->next = tail;
		tail->addr.allocate = PAGEADDR_NO_ALLOC;
		tail->flags.olkty = PAGEFLAGS_OLKTY_BAFH;
		tail->next = mlist;
		return TRUE;
	}
	
	return FALSE;
}


/**
 * 从MemList中查找与(head,tail)连续的subpages，并将它们连接在一起
 */
static bool_t get_continue_subpages(MemList *mlist,MemPage **head,MemPage **tail){
	list_t* pos = NULL;
	MemPage *start = NULL,*end=NULL;
	//依次遍历
	list_for_each(pos,&mlist->free_pages){
		start = list_entry(pos,MemPage,hook);
		end = (MemPage*)start->next;
		//两种连续的情况
		if((*tail)->addr.value+1==start->addr.value){	//后端连续:与tail连续
			//从链表中取出指定的subpages，并修改标志位
			list_del(pos);
			mlist->free_num--;
			mlist->total_num--;
			
			//合并成新的subpages
			(*head)->next = end;
			*tail = end;
			return TRUE;
		}else if(end->addr.value+1==(*head)->addr.value){//前端连续:与head连续
			//从链表取出指定的subpages，并修改标志位
			list_del(pos);
			mlist->free_num--;
			mlist->total_num--;
			
			//合并成新subpages
			start->next = tail;
			*head = start;
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * 根据MemList区间执行分割合并操作
 */
static bool_t merge_execute(MemArea *area,MemPage* pages,size_t size,MemList *startList,MemList *endList){
	MemPage *startPage = pages,*endPage=&pages[size-1];
	//设置subpage的信息，完成释放，并根据返回值确定是否需要进一步的合并操作
	bool_t rets = merge_update_subpages(startList,pages,size);

	if(rets==FALSE){//subpage仍被占用不需要合并
		return TRUE;
	}
	//从低阶MemList开始，依次向高阶进行合并
	for(; startList<endList; startList++){
		if(get_continue_subpages(startList,&startPage,&endPage)==FALSE){
			break;
		}
	}

	//将最终合并出来的内存添加到list中
	put_subpages(startList,startPage,endPage);

	return TRUE;
}


/**
 * the core function of memory merge
 */
static bool_t merge_core(MemArea *area,MemPage *pages,size_t size){
	bool_t rets = FALSE;
	//回收应用区内存
	if(MEMAREA_TYPE_APPS == area->type){
		if(size>1){
			//只能回收单个页面
			return FALSE;
		}
		MemList *list = &area->table.list;
		merge_update_subpages(list,pages,size);//更新待合并subpage信息
		//将待合并子串挂入MemList
		list_add(&pages->hook,&list->free_pages);
		list->free_num++;
		list->total_num++;
		
		//更新内存区信息
		area->free_num++;
		area->allocate_num--;
	}else{
		//获取需要处理的链表范围
		MemList *startList = NULL,*endList=NULL;
		if(merge_list_range(area,size,&startList,&endList)==FALSE){
			return FALSE;
		}
		
		//执行合并任务
		rets = merge_execute(area,pages,size,startList,endList);
	}

	return rets;
}


/**
 * the frame function of memory merge
 */
static bool_t merge_frame(memmgr_t *mgr,MemPage *pages,size_t size){
	//获取内存区
	MemArea *area = get_memArea(mgr,pages[0].flags.marty);
	if(NULL==area){
		return FALSE;
	}
	
	cpuflag_t cpuflag = 0;
	//内存区上锁并关中断
	spinlock_lock_cli(&area->lock,&cpuflag);

	//调用核心函数执行
	bool_t rets = merge_core(area,pages,size);

	//内存区解锁并开中断
	spinlock_unlock_sti(&area->lock,&cpuflag);

	return rets;
}


/**
 * the interface function of memory merge
 * mgr: Memmgr struct 
 * pages：the continued pages to be released 
 * pages：the length of pages
 */
bool_t mem_merge(memmgr_t *mgr,MemPage *pages,size_t size){
	//parameters tesing
	if(NULL==mgr || NULL==pages || 0==size){
		return FALSE;
	}

	//invoking the frame function
	bool_t rets =  merge_frame(mgr,pages,size);

	//result testing
	return rets;
}

//============================================================================================
INLINE void test_get_order();
INLINE void test_get_memArea();
INLINE void test_divide_list_range();
INLINE void test_merge_list_range();
INLINE void test_mem_divide();
INLINE void test_mem_merge();
/**
 * 测试主函数
 */
void memapi_test_main(){
	//test_get_order();	
	//test_get_memArea();
	//test_divide_list_range();
	//test_merge_list_range();
	//test_mem_divide();
	//test_mem_merge();
	
	//die(0);
	return;	
}

INLINE void test_get_order(){
	
	return;
}

INLINE void test_get_memArea(){
	return;
}

INLINE void test_divide_list_range(){
	MemList *start=NULL,*end=NULL;
	MemArea *area = &memmgr.areas[1];
	size_t size = 400;
	bool_t rets = divide_list_range(area,size,&start,&end);
	printk("rets:%d\n",rets);
	printk("realSize:%d\n",end-start+1);
	printk("start order:%d\n",start->order);
	printk("end order:%d\n",end->order);

	memlists_tostring(area->table.lists);
	
	return;
}


INLINE void test_merge_list_range(){
	
	
	return;
}

INLINE void test_mem_divide(){
	//穷举测试
	//将所有内存分配光
	return;
}

INLINE void test_mem_merge(){
	MemPage *subpage = NULL;
	size_t realSize=0;
	list_t head;
	list_t_init(&head);
	
	memlists_tostring(memmgr.areas[1].table.lists);	
	do{
		subpage = mem_divide(&memmgr,16,&realSize,MEMAREA_TYPE_KRNL,MEMTABLE_RELDIV);
		if(NULL!=subpage)
			list_add(&subpage->hook,&head);
	}while(NULL!=subpage);

	memlists_tostring(memmgr.areas[1].table.lists);	
	
	list_t *pos = NULL;
	list_for_each_head_dell(pos,&head){
		subpage = list_entry(pos,MemPage,hook);
		list_del(&subpage->hook);
		mem_merge(&memmgr,subpage,16);
	}
	
	memlists_tostring(memmgr.areas[1].table.lists);
	printk("successfully pass test_mem_merge test\n");
	
	return;
}
