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
#define STACK_ADR		0x90000U-0x10		//stack:内核栈地址
/*
 * x86架构中，访存以16字节为单位，所以内核栈地址需要16字节对齐，
 * 又因为栈向低地址拓展，如1KB的栈空间为[8F000,8F010,8F020,...,8FFE0,8FFF0]
 * 栈地址是内核栈最后一个元素的地址：0x8FFF0:0x90000-0x10
 */
//1MB之后的地址
#define MACHINE_ADR		0x100000U			//machine_t:机器信息结构体machine_t地址
#define LOADER_ADR 		0x200000U			//loader.bin:二级引导器地址
#define MMU_ADR		0x1000000U		//MMU页表
#define KERNEL_ADR 		0x2000000U		//kernel.bin:内核地址(低32MB将来会回收并保留给硬件使用)
#define IMAGE_ADR 		0x4000000U		//Wind.kif(grub.bin引导):映像文件地址

/* 内核地址空间 */
#define KERNEL_VIRADR_START 	0xffff800000000000ULL	//内核起始虚拟地址(高17为全1)
#define KERNEL_VIRADR_SIZE	0x400000000ULL			//内核虚拟地址大小:16GB

typedef struct MACHINE Machine;	//声明机器信息结构体

/* 硬件模块聚合头文件 */
#include "type.h"       //全局类型定义
#include "stdio.h"		//
#include "string.h"		//
#include "image.h"		//

#include "vgastr.h"		//文本模式处理
#include "mmu.h"		//MMU处理模块
#include "cpu.h"     	//CPU处理模块
#include "graph.h"		//显卡处理模块
#include "acpi.h"		//acpi处理模块
#include "e820.h"		//e820数组模块
#include "disk.h"		//disk管理模块
#include "machine_t.h"	//机器信息结构体
#include "machine.h"	//机器信息结构


#endif //__GLOBAL_H