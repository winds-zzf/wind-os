/**
 * 内存表初始化
 */

#include "globaltype.h"
#include "globalctrl.h"

static void init_memtable(MemTable *table);

void memtable_init(MemArea *area){
	//basic initiatkizations for MemTable array
	init_memtable(&area->table);

	return ;
}

/**
 * 初始化并设置MemList的值
 * list：MemList结构体他指针
 * status: 状态
 * order: 阶
 */
static void set_memlist(MemList *list,uint_t status,uint_t order){
	spinlock_init(&list->lock);
	list->status = status;
	list->order = order;
	list->size = 0x1<<order;
	list->total_num = 0;
	list->free_num = 0;
	list->allocate_num = 0;
	list->recycle_num = 0;
	//initializing pages list
	list_t_init(&list->free_pages);
	list_t_init(&list->allocate_pages);
	
	return;
}


/**
 * 初始化MemTable结构
 * table: MemTable's pointer
 */
static void init_memtable(MemTable *table){
	spinlock_init(&table->lock);
	table->status = 0;
	table->allocate_num = 0;
	table->recycle_num = 0;
	for(uint_t i=0;i<MEMLIST_MAX;i++){
		//统一使用DIVM状态：分割合并状态
		set_memlist(&table->lists[i],BAFH_STUS_DIVM,i);
	}
	//对于单个链表使用ONEM状态
	set_memlist(&table->list,BAFH_STUS_ONEM,1);
	
	return;
}

/**
 * 检查指定页面是空闲,是否属于flags指定的区域
 * pages：待检查的内存页
 * flags：负责传递内存区域类型
 * prior：上一个内存物理块号，用于判断相邻两个内存页在物理上是否连续
 */
static bool_t match_page(MemPage* page,PageFlags flags,addr_t prior){
	//判断地址是否连续(前提)
	if(prior+1 == page->addr.value){
		//判断（是否属于指定内存区&&是否空闲）
		if(flags.marty == page->flags.marty			//属于指定内存区
		&& PAGEADDR_NO_ALLOC == page->addr.allocate		//内存地址尚未分配
		&& PAGEFLAGS_UINDX_INIT == page->flags.count	//分配计数为0
		&& PAGEFLAGS_MOCTY_FREE == page->flags.mocty){	//内存页尚未分配
			return TRUE;
		}
	}
	return FALSE;
}

/**
 * 连续分配
 * 原理：len的二进制形式可以表示成多项式形式，每一项都是一个2阶连续子页表：1表示该子串存在，0表示该子串不存在
 */
static void split_continuous(MemArea* area,MemPage* head,size_t len){
	MemList *lists = area->table.lists;	//获取MemList数组
	MemPage *tail = NULL;
	MemList *list = NULL;
	size_t size = 0;
	//
	for(sint_t offset = MEMLIST_MAX-1; offset >= 0; offset--){
		if( (len>>offset)&0x1UL ){	//检查len的第offset位是否为1（第i位右移i位回到0位）
			//获取子串(head,size,tail)和挂载点list
			head += size;			//获取子串头节点指针
			size = 0x1UL<<offset;	//计算新子串长度
			tail = head+size-1;		//获取子串尾节点指针
			list = lists+offset;	//获取挂载点list
			
			//1.验证MemList的状态不能挂在单节点链表(ONEM)上
			if(BAFH_STUS_DIVP!=list->status && BAFH_STUS_DIVM!=list->status) continue;
			
			//2.建立连接并修改标志位
			head->flags.olkty = PAGEFLAGS_OLKTY_ODER;
			head->next = tail;
			tail->flags.olkty = PAGEFLAGS_OLKTY_BAFH;
			tail->next = list;
			
			//3.挂载并修改Memlist信息
			list_add(&head->hook,&list->free_pages);
			list->total_num++;
			list->free_num++;
			
			//4.修改MemArea信息
			area->total_num += size;
			area->free_num += size;
		}
	}
	
	return;
}

/**
 * 懒分配
 * theory：split continuous pages into single list of pages
 */
static void split_single(MemArea *area,MemPage* head,size_t len){
	MemList* list = &area->table.list;	//获取单个使用的MemList
	printk("type:%d\n",area->type);
	for(size_t i=0; i<len; i++){
		//1.验证MemList的状态：单节点链表
		if(BAFH_STUS_ONEM != list->status) continue;
		
		//2.建立连接并修改标志位
		head[i].flags.olkty = PAGEFLAGS_OLKTY_BAFH;		//Why is it defined like this？
		head[i].next = list;
		
		//3.挂载并修改MemList信息
		list_add(&head[i].hook,&list->free_pages);
		list->total_num++;
		list->free_num++;

		//4.修改MemArea信息
		area->total_num++;
		area->free_num++;
	}
	
	return;
}


/**
 * 根据连续内存页的具体类型选择选择对应的的回调函数进行分割合并
 * area:
 * head:
 * tail:
 */
static bool_t mount(MemArea *area,MemPage* head,MemPage* tail){
	//根据不同的内存区使用不同的回调函数处理
	switch(area->type){
	case MEMAREA_TYPE_INIT:
		break;
	case MEMAREA_TYPE_HWAD:
		split_continuous(area,head,tail-head+1);
		break;
	case MEMAREA_TYPE_KRNL:	
		split_continuous(area,head,tail-head+1);
		break;
	case MEMAREA_TYPE_APPS:
		split_single(area,head,tail-head+1);
		break;
	case MEMAREA_TYPE_SHAR:
		break;
	default:
		break;
	}
	
	return TRUE;
}


/**
 * 获取下一个最大连续子页表
 * flags:
 * next:
 * left:
 * head:
 */
static size_t Next(PageFlags flags,MemPage **next,size_t *left,MemPage **head){
	*head = NULL;		//既做头指针，也做标志位
	//标志优先判断：跳过0个或多个不符合标志的page
	while(*left>0 && !match_page(*next,flags,(*next)->addr.value-1)){
		(*next)++;
		(*left)--;
	}
	if(*left<=0) return 0;
	else *head = *next;	//首次设置head值，子表首元素地址
	//标志优先判断
	addr_t prior = (*next)->addr.value-1;
	while(*left>0 && match_page(*next,flags,prior)){
		(*next)++;	//指针后移
		(*left)--;	//剩余数量减1
		prior++;		//页号加1(因为连续)
	}
	
	return *next-*head;
}


/**
 * 
 */
void mount_table(MemArea* area,MemPage* pages,size_t len){
	MemPage *head = NULL,*next=pages;		//next指向下一个待处理页
	PageFlags flags;
	flags.marty = area->type&0x3;			//标志位设置为内存区类型
	
	//按照顺序获取子串
	while(Next(flags,&next,&len,&head)>0){
		//将子串挂载到内存区指定链表下
		mount(area,head,next-1);
		//printk("subpages:0x%lx,0x%lx,0x%lx\n",head->addr.value,(next-1)->addr.value,next-head);
	}
	
	printk("\narea total_pages:0x%lx,0x%lx\n",area->total_num,area->free_num); 
	return;
}

/**
 * 输出area的属性信息，每个子链表有多少个节点
 * 将面向对象编程的思想应用到这里
 */
void memlists_tostring(MemList *lists){
	for(uint_t i=0;i<MEMLIST_MAX;i++){
		printk("%d ",lists[i].free_num);
	}
	printk("\n");
	return;
}

//=============================================================================================================
INLINE void test_mount(MemArea* area);

void memlist_test_main(){

	die(0);
	return ;
}

INLINE void test_mount(MemArea* area){
	MemList* lists = area->table.lists;
	MemList* list = &area->table.list;
	printk("area total_pages:0x%lx,0x%lx\n",area->total_num,area->free_num); 
	//链表数组
	for(uint_t i=0;i<52;i++){
		if(lists[i].total_num>0)
			printk("0x%lx,0x%lx,0x%lx\n",lists[i].order,lists[i].total_num,lists[i].free_num);
	}
	//单链表
	printk("0x%lx,0x%lx,0x%lx\n",list->order,list->total_num,list->free_num);
	return;
}
























