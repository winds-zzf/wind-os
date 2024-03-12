/**
 * 全局变量定义
 */

#include"globaltype.h"
#include"globalctrl.h"


//平台管理基础
machine_t machine;			//定义全局机器信息结构体变量


//中断管理基础
IntGate idt[INTMAX];		//定义中断门描述符表
idtr_t idtr;				//定义中断门描述符表寄存器
IntFault intFaults[INTMAX];	//定义中断异常描述符表

/**
 * 每颗CPU都对应一个TR和一个GDTR，其中
 * TR：指向TSS
 * GDTR：指向GDT
 * 因此，每颗CPU都有一个GDT表和TSS段
 */
Segment gdt[CPUCORE_MAX][GDTMAX];
gdtr_t gdtr[CPUCORE_MAX];		//定义全局段描述符表寄存器
tss_t x64tss[CPUCORE_MAX];


//物理内存管理结构
memmgr_t memmgr;
