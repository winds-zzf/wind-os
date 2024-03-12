/**
 * 系统服务模块
 * 类型定义
 */
#ifndef __SERVICE_T_H
#define __SERVICE_T_H


#define SYSSTUSERR -1
#define SYSSTUSOK 0
/**
 * 这个前缀是使用SYSCALL_好还是使用DERVICE_
 */
#define SERVICE_MAX 2	//最大内核服务接口数量
/* 系统调用号这里必须使用long型常量 */
#define SERVICE_NONE 0ULL	//0号系统调用
#define SERVICE_TIME 1ULL	//1号系统调用

























/**
 * 栈参数结构
 * 解析栈帧指针指向的数据
 */
typedef struct SFRAME_T{
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
	u64_t	parm5;	//rdi
	u64_t	parm4;	//rsi
	u64_t	rbp;
	u64_t	parm3;	//rdx
	u64_t	parm2;	//rcx
	u64_t	parm1;	//rbx
	u64_t	rvsrip;
	u64_t	rvscs;
	u64_t	rvsrflags;
	u64_t	rvsrsp;
	u64_t	rvsss;
}sframe_t;


/* 服务函数类型 */
typedef sysstus_t (*syscall_t)(uint_t inr,sframe_t* sframe);


/* 声明全局内核服务表 */
extern syscall_t osservicetab[SERVICE_MAX];


#endif //__SERVICE_T_H