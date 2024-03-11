/**
 * 内存管理
 * API定义
 */
#ifndef __MEMMGR_H
#define __MEMMGR_H


#include  "memmgr_t.h"

/**
 * 物理内存管理器初始化
 */
void memmgr_init(machine_t *mach);

/**
 * 
 */
void memmgr_display(machine_t* mach);


#endif //__MEMMGR_H