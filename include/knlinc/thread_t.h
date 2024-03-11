/**
 * 进程管理模块
 * 类型定义
 */
#ifndef __THREAD_T_H
#define __THREAD_T_H

#include "tss_t.h"
#include "objnode_t.h"


/* 进程五状态模型状态 */
#define THREAD_STATUS_NEW	0
#define THREAD_STATUS_READY	1
#define THREAD_STATUS_WAIT	2
#define THREAD_STATUS_RUN	3
#define THREAD_STATUS_END	4

/* 进程标志 */
#define THREAD_FLAG_FREE		(1)
#define THREAD_FLAG_BUSY		(2)

/* 进程特权级 */
#define THREAD_PRIVILEGE_SYS	0
#define THREAD_PRIVILEGE_USR	1

/* 进程优先级 */
#define THREAD_PRIORITY_MAX 64	//最大优先级
#define THREAD_PRIORITY_MIN 0

/* 进程时间片 */
#define THREAD_RUN_TICK 20

/* 进程类型 */
#define THREAD_TYPE_KRNL 0
#define THREAD_TYPE_USER 1

/* 进程默认栈大小 */
#define THREAD_STACKSIZE_KRNL 0x8000	//默认内核栈大小
#define THREAD_STACKSIZE_USER 0x8000	//默认应用栈大小

/* 进程最大占用的资源描述符数量 */
#define THREAD_HAND_MAX 	8

/*  */
#define DAFT_SPSR 		0x10
#define DAFT_CPSR 		0xd3
#define DAFT_CIDLESPSR 	0x13 

/* 段描述符的GDT索引 */
#define KRNL_CS_IDX		0x08		//内核代码段索引(R0)
#define KRNL_DS_IDX		0x10		//内核数据段索引(R0)
#define USER_CS_IDX		0x1b		//应用代码段索引(R3)
#define USER_DS_IDX		0x23		//应用数据段索引(R3)
/*  */
#define KRNL_TAR_IDX	0x28		//

/*  */
#define USERMOD_EFLAGS	0x1202	//user mode eflags
#define KRNLMOD_EFLAGS	0x0202	//kernel mode eflags


/* 进程上下文信息 */
typedef struct CONTEXT_T{
	u64_t	nextrip;		//保存下一次运行的地址(下一次运行的指令地址)
	u64_t	nextrsp;		//保存下一次运行时内核栈的地址(栈顶位置)
	tss_t*	nexttss;		//指向tss结构
}context_t;


/* 进程结构 */
typedef struct THREAD_T{
	spinlock_t 		lock;		//保护结构的自旋锁
	list_t			hook;		//挂载进程的钩子
	u64_t		flags;		//进程标志
	u64_t		status;		//进程状态（创建,就绪,运行,阻塞,终止）
	//
	u64_t		cpuid;		//进程所在的CPU的id
	u64_t		id;			//进程的id(进程描述符地址)
	u64_t		tick;		//CPU时间片
	u64_t		privilege;	//进程的权限(用户进程|内核进程)
	u64_t		priority;		//进程的优先级(64个优先级，数值越小，优先级越高)
	u64_t		runmode;		//进程的运行模式
	//进程栈信息（每个进程栈是独立的）
	addr_t		krlstktop;	//进程内核栈顶地址
	addr_t		krlstkbase;	//进程内核栈基地址
	addr_t		usrstktop;	//进程应用栈顶地址
	addr_t		usrstkbase;	//进程应用栈基地址
	//
	KvmProcess*	mem;			//地址空间结构（每个进程内核部分是共用的，应用部分是独立的）
	context_t		context;		//机器上下文结构（用于进程切换）
	objnode_t*	objs[THREAD_HAND_MAX];	//进程打开的所有资源描述符，最多TD_HAND_MAX个
}thread_t;


/**
 * x86CPU长模式下rsp栈指针寄存器始终是8字节对齐
 * GCC编译器小端模式：
 */
typedef struct INTSTKREGS_T{
	//低字节低地址端
	/* 通用寄存器是在中断入口函数中通过代码保存到栈中的 */
	u64_t	gs;
	u64_t	fs;
	u64_t	es;
	u64_t	ds;
	u64_t	r15;
	u64_t	r14;
	u64_t	r13;
	u64_t	r12;
	u64_t	r11;
	u64_t	r10;
	u64_t	r9;
	u64_t	r8;
	u64_t	rdi;
	u64_t	rsi;
	u64_t	rbp;
	u64_t	rdx;
	u64_t	rcx;
	u64_t	rbx;
	u64_t	rax;
	/** 
	 * 特殊寄存器是CPU自动压入栈中的
	 * 为什么是old? 
	 */
	//Code
	u64_t	rip_old;	//程序指针寄存器	
	u64_t	cs_old;	//代码段寄存器
	//CPU
	u64_t	rflgs;	//rflags寄存器
	//Stack
	u64_t	rsp_old;	//栈指针寄存器
	u64_t	ss_old;	//堆栈段寄存器
	//高字节高地址端
}intstkregs_t;


#endif //__THREAD_T_H