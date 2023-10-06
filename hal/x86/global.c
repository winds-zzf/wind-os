/**
 * 全局变量定义
 */

#include"globaltype.h"
#include"globalctrl.h"


//平台管理基础
Machine machine;			//定义全局机器信息结构体变量


//中断管理基础
IntGate idt[INTMAX];		//定义中断门描述符表
Idtr idtr;				//定义中断门描述符表寄存器

Segment gdt[CPUCORE_MAX][GDTMAX];
Gdtr gdtr[CPUCORE_MAX];		//定义全局段描述符表寄存器

IntFault intFaults[INTMAX];	//定义中断异常描述符表

tss_t x64tss[CPUCORE_MAX];		//任务段Task Segment


//内存管理基础结构
Memmgr memmgr;
