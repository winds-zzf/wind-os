/**
 * 内存页结构
 * API定义
 */

#ifndef __MEMPAGE_H
#define __MEMPAGE_H

#include "mempage_t.h"


/**
 * 初始化内存页
 */
void mempage_init();

/**
 * 表示占用的内存
 */
void mempage_occupy();

/**
 * 获取页描述符对应物理页的虚拟地址
 */
INLINE addr_t mempage_vadr(MemPage *page){
	if(NULL==page){
		return NULL;
	}
	return MAP_RV(page->addr.value<<12);
}

/**
 * 获取页描述符对应物理页的物理地址
 */
INLINE addr_t mempage_padr(MemPage *page){
	if(NULL==page){
		return NULL;
	}
	return page->addr.value<<12;
}

/**
 * 获取subpage的长度
 */
INLINE size_t mempage_len(MemPage *head){
	if(NULL==head||NULL==head->next){
		return 0;
	}
	MemPage *tail = (MemPage*)head->next;
	if(tail<head){
		return 0;
	}
	return tail-head+1;
}


#endif //__MEMPAGE_H