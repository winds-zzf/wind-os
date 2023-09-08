/**
 * 内存区
 * API定义
 */

#ifndef __MEMTABLE_H
#define __MEMTABLE_H

#include "memtable_t.h"

/**
 * 初始化内存表
 */
void memtable_init(MemTable* table);


/**
 * 显示内存页表
 */
void memtable_display();



#endif //__MEMTABLE_H