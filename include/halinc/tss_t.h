/**
 * 任务状态段
 * 类型定义
 */
#ifndef __TSS_T_H
#define __TSS_T_H


/**
 * 任务状态段：Task State Segment 
 * 用于存储任务的状态信息和上下文切换所需的信息，在x86架构的操作系统中，每个任务都有对应的TSS
 * 每一个CPU核心都有一个tss
 */
typedef struct TSS_T{
	u32_t	reserve0;		//保留
	u64_t	rsp0;		//R0特权级的栈地址
	u64_t	rsp1;		//R1特权级的栈地址，未使用
	u64_t	rsp2;		//R2特权级的栈地址，未使用
	u64_t	reserve28;	//保留
	u64_t	ist[7];		//不使用
	u64_t	reserve92;	//保留
	u16_t	reserve100;	//保留
	u16_t	iobase;		//未使用
}__attribute__((packed))tss_t;

/*  */
extern tss_t x64tss[CPUCORE_MAX];


#endif //__TSS_T_H