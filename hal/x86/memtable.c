/**
 * 内存表初始化
 */

#include "globaltype.h"
#include "globalctrl.h"


/**
 * 初始化并设置MemList的值
 * list：MemList结构体他指针
 * status: 状态
 * order: 阶
 */
static void set_memlist(MemList *list,uint_t status,uint_t order){
	knl_spinlock_init(&list->lock);
	list->status = status;
	list->order = order;
	list->total_num = 0;
	list->free_num = 0;
	list->allocate_num = 0;
	list->recycle_num = 0;
	//
	list_init(&list->free_pages);
	list_init(&list->allocate_pages);
	return;
}



/**
 * 初始化MemTable结构
 * table: MemTable指针
 */
static void  init_memtable(MemTable *table){
	knl_spinlock_init(&table->lock);
	table->status = 0;
	table->allocate_num = 0;
	table->recycle_num = 0;
	for(uint_t i=0;i<MEMLIST_MAX;i++){
		set_memlist(&table->lists[i],MEMLIST_STATUS_DIVM,i);
	}
	set_memlist(&table->list,MEMLIST_STATUS_ONEM,1);
	return;
}

void memtable_init(MemTable* table){
	init_memtable(table);
	return ;
}


void memtable_display(){
	
	
	
	return;
}
