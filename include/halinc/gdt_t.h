/**
 * GDT初始化模块
 * 类型定义
 * x86CPU 长模式 
 */
#include "platform_t.h"

#ifndef __GDT_T_H
#define __GDT_T_H

#define GDTMAX 10		//最大全局段描述符数

/* privilege level R[0-3] */
#define PRIVILEGE_KRNL 0		//内核权限级别R0(最高的特权级)
#define PRIVILEGE_TASK 1		//任务权限级别R1
#define PRIVILEGE_USER 3		//用户权限级别R3(最低的特权级)

/* segment selector */
#define	SELECTOR_DUMMY		0x00		//虚设段选择子(用作占位符)
#define	SELECTOR_FLAT_C	0x08		//代码段选择子
#define	SELECTOR_FLAT_RW	0x10		//可读写的数据段选择子

#define	SELECTOR_KERNEL_CS	SELECTOR_FLAT_C	//内核代码段选择子
#define	SELECTOR_KERNEL_DS	SELECTOR_FLAT_RW	//内核数据段选择子

/* 描述符类型值说明 */
#define 	DA_64 			0x2000	/*  */
#define	DA_32			0x4000	/* 32 位段	 */
#define	DA_LIMIT_4K		0x8000	/* 段界限粒度为 4K 字节 */
#define	DA_DPL0			0x00		/* DPL = 0 */
#define	DA_DPL1			0x20		/* DPL = 1 */
#define	DA_DPL2			0x40		/* DPL = 2 */
#define	DA_DPL3			0x60		/* DPL = 3 */
/* 存储段描述符类型值说明 */
#define	DA_DR			0x90		/* 存在的只读数据段类型值 */
#define	DA_DRW			0x92		/* 存在的可读写数据段属性值 */
#define	DA_DRWA			0x93		/* 存在的已访问可读写数据段类型值 */
#define	DA_C				0x98		/* 存在的只执行代码段属性值 */
#define	DA_CR			0x9A		/* 存在的可执行可读代码段属性值	*/
#define	DA_CCO			0x9C		/* 存在的只执行一致代码段属性值	*/
#define	DA_CCOR			0x9E		/* 存在的可执行可读一致代码段属性值 */
/* 系统段描述符类型值说明 */
#define	DA_LDT			0x82		/* 局部描述符表段类型值 */
#define	DA_TaskGate		0x85		/* 任务门类型值,64位模式下不存在任务门*/
#define	DA_386TSS			0x89		/* 可用 386 任务状态段类型值 */
#define	DA_386CGate		0x8C		/* 386 调用门类型值 */
#define	DA_386IGate		0x8E		/* 386 中断门类型值 */
#define	DA_386TGate		0x8F		/* 386 陷阱门类型值 */


/* 段描述符 */
typedef struct SEGMENT{
	u16_t	limit_low;		//Limit
	u16_t	base_low;			//Base:
	u8_t		base_mid;			//Base:
	u8_t		attr1;			//P(1) DPL(2) DT(1) TYPE(4)
	u8_t		limit_high_attr2;	//G(1) D(1) 0(1) AVL(1) LimitHigh(4)
	u8_t		base_high;		//Base:
}__attribute__((packed))Segment;	//8B
/* 全局段描述符表 */
extern Segment gdt[CPUCORE_MAX][GDTMAX];

/* 全局段描述符表头 */
typedef struct GDTR_T{
	u16_t 	len;				//长度
	u64_t 	base;			//基址
}__attribute__((packed))gdtr_t;		//10B

/* GDTR */
extern gdtr_t gdtr[CPUCORE_MAX];


#endif //__GDT_T_H