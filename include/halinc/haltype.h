/**
 * hal层聚集类型头文件
 */
#ifndef __HALTYPE_H
#define __HALTYPE_H


//平台模块
#include "platform_t.h"

//中断模块
#include "gdt_t.h"
#include "tss_t.h"
#include "idt_t.h"
#include "i8259_t.h"
#include "interrupt_t.h"
#include "cpuctrl_t.h"

//内存模块
#include "remmu_t.h"   //待优化模块
#include "mmu_t.h"
#include "memview_t.h"
#include "mempage_t.h"
#include "memtable_t.h"
#include "memarea_t.h"
#include "memobject_t.h"	
#include "memmgr_t.h"
#include "memapi_t.h"	


#endif //__HALTYPE_H