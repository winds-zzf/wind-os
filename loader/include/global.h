/**
 * 全局定义头文件
 */
#ifndef __GLOBAL_H
#define __GLOBAL_H


/* 全局映像名称定义 */
#define IMAGE_NAME 		"Wind.kif"		//内核映像文件名
#define LOADER_NAME 	"loader.bin"		//二级引导器模块名
#define BIOS_NAME 		"bios.bin"		//BIOS中断调用名
#define KERNEL_NAME 	"kernel.bin"		//内核模块名
#define FONT_NAME		"font.ttf"		//字体文件名
#define LOGO_NAME		"logo.bmp"		//logo文件名

/* 全局常量定义 */
#define STACK_SIZE		0x1000U			//stack:内核栈大小1KB
#define BIOS_ADR 		0x1000U			//bios.bin:BIOS调用模块地址  
#define E820S_ADR		0x20064U			//e820s:e820数组地址
#define BITMAP_ADR		0x30000U			//bitmap:位示图地址
#define STACK_ADR		0x90000U			//stack:内核栈地址
//1MB之后的地址
#define MACHINE_ADR		0x100000U			//machine:机器信息结构体machine地址
#define LOADER_ADR 		0x200000U			//loader.bin:二级引导器地址
#define PAGE_ADR		0x1000000U		//page:MMU页表
#define KERNEL_ADR 		0x2000000U		//kernel.bin:内核地址
#define IMAGE_ADR 		0x4000000U		//Wind.kif(grub.bin):映像文件地址

/* 内核地址空间 */
#define KERNEL_VIRADR_START 	0xffff800000000000ULL				//内核起始虚拟地址
#define KERNEL_VIRADR_SIZE	0x400000000ULL						//内核虚拟地址大小:16GB
#define KERNEL_VIRADR_END 	(KERNEL_VIRADR_START+KERNEL_VIRADR_END-1)	//内核结束虚拟地址

/* 通用聚合头文件 */
#include "type.h"       //全局类型定义
#include "lib.h"		//标准库函数

typedef struct MACHINE Machine;	//声明机器信息结构体

/* 硬件模块聚合头文件 */
#include "vgastr.h"		//文本模式处理
#include "mmu.h"		//MMU处理模块
#include "cpu.h"     	//CPU处理模块
#include "graph.h"		//vbe模式模块
#include "acpi.h"		//acpi处理模块
#include "e820.h"		//e820数组模块
#include "disk.h"		//disk管理模块
#include "machine_t.h"	//机器信息结构体
#include "machine.h"	//机器信息结构


#endif //__GLOBAL_H