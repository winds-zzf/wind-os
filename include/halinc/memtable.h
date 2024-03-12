/**
 * 内存区
 * API定义
 */

#ifndef __MEMTABLE_H
#define __MEMTABLE_H

#include "memarea_t.h"
#include "mempage_t.h"

/**
 * 初始化内存表
 */
void memtable_init(MemArea* areas);

/**
 * 
 */
void mount_table(MemArea* area,MemPage* pages,size_t len);

/**
 * 添加一些操作list的结构，比如增加子串，删除子串
 */


/**
 * 添加子串
 */
void memlist_put();


/**
 * 取出子串
 */
void memlist_get();

/**
 *
 */
void memlists_tostring(MemList *lists);

#endif //__MEMTABLE_H