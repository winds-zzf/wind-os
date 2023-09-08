/**
 * 任务状态段
 * 类型定义
 */
#ifndef __TSS_T_H
#define __TSS_T_H


/* TSS:任务状态段Task State Segment */
typedef struct TSS{
	u32_t reserv0;		//
	u64_t rsp0;		//
	u64_t rsp1;		//
	u64_t rsp2;		//
	u64_t reserv28;	//
	u64_t ist[7];		//
	u64_t reserv92;	//
	u16_t reserv100;	//
	u16_t iobase;		//
}__attribute__((packed))Tss;
/*  */
extern Tss x64tss[CPUCORE_MAX];


#endif //__TSS_T_H