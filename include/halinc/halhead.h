/**
 * hal层聚集头文件
 */
#ifndef __HALHEAD_H
#define __HALHEAD_H

//通用
#include "type.h"			//全局基本类型
#include "lib.h"			//通用库文件
#include "vgastr.h"			//文本模式

//平台
#include "platform.h"		//平台初始化模块

//中断
#include "io.h"			//端口输入输出头文件
#include "gdt.h"			//gdt初始化模块
#include "tss.h"			//任务状态段模块
#include "idt.h"			//idt初始化模块
#include "i8259.h"			//8259A中断控制模块
#include "interrupt.h"		//中断初始化模块
#include "cpuctrl.h"		//CPU控制模块
#include "spinlock.h"		//自旋锁模块

//内存
#include "remmu.h"			//内存重映射模块（待优化）
#include "mmu.h"   			//mmu管理模块
#include "memview.h"		//内存初始化模块
#include "mempage.h"		//内存页模块
#include "memtable.h"		//内存页链表
#include "memarea.h"		//内存区模块
#include "memobject.h"		//内存对象模块
#include "memmgr.h"			//内存管理模块
#include "memapi.h"			//内存接口模块


#endif //__HALHEAD_H