/**
 * 任务状态段
 * 类型定义
 */
#ifndef __TSS_T_H
#define __TSS_T_H


/**
 * 任务状态段：Task State Segment 
 *
 * x64的TSS Segment唯一的作用就是提供栈指针
 * long mode 已经不支持使用 TSS 机制进行任务切换。
 * TSS 必须存在的唯一原因依然是：需要提供每个权限级别的 stack 指针。
 * 但是，由于不支持TSS任务切换机制，所以，long mode下的TSS segment 和原来的TSS segment有很大的改变。
 * long mode 下的TSS segment中的stack pointer 被扩展至 64 位，SS selector 被抛弃。
 * 同时又增加了7 个stack pointer。
 * 这些新增的7 个stack pointer 和原来的stack pointer 意义和用途都不同 。
 * 原来的3 个stack pointer 意义是代表权限级别为0、1、2 的通用stack pointer，
 * 在任务切换时，若发生了stack 切换时相应级别的 stack pointer。
 * 新增的7个stack pointer 是特定用途的 stack pointer，仅用于发生中断和异常时使用。
 * 这7个stack pointer 意即：Interrupt Stack Table（IST）IST ~ IST7。
 * 使用于interrupt或 trap gate descriptor 中。
 * 在 interrupt/trap gate descriptors 里指出相应的stack pointer号，从而进入相应的 stack。
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