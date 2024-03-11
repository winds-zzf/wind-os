/**
 * 基本数据
 * 类型定义
 * 说明：
 * 64位长模式下的类型定义
 */
#ifndef __TYPE_T_H
#define __TYPE_T_H


//无符号数
typedef unsigned char u8_t;	
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef unsigned long u64_t;

//有符号数
typedef signed char s8_t;
typedef signed short s16_t;
typedef signed int s32_t;
typedef signed long s64_t;

typedef unsigned int uint_t;	//无符号int
typedef signed int sint_t;	//有符号int
typedef signed long slong_t;	//有符号长整数
typedef unsigned long ulong;	//无符号长整数

//专用类型
typedef u64_t bool_t;		//使用u64_t以提供最大的兼容性
typedef u64_t addr_t;		//地址类型
typedef u64_t size_t;		//大小类型
typedef char char_t;		//字符类型
typedef u8_t byte_t;		//子节类型

//硬件类型
typedef u64_t cpuflag_t;		//CPU标志类型
typedef s64_t drvstus_t;		//驱动状态类型
typedef s64_t sysstus_t;		//系统状态类型
typedef u64_t memstus_t;		//
typedef s64_t hand_t;		//
typedef void* buf_t;		//缓冲区类型


//宏定义
#define PUBLIC			//公有域：default
#define PRIVATE static 	//私有域
#define EXTERN extern	//extern
#define INLINE static inline 		//静态内置函数
#define NULL 0			//空指针
#define TRUE 1			//true
#define FALSE 0		//falze

#define ALIGN(x,b) (((x)+(b)-1)&~((b)-1))	//x向上a倍对齐
#define ALIGN_4K(x) ALIGN(x,0x1000)		//向上4K对齐


//定义错误类型
#define	EOK 		0
#define	EPERM	1	/* Operation not permitted */
#define	ENOENT	2	/* No such file or directory */
#define	ESRCH	3	/* No such process */
#define	EINTR	4	/* Interrupted system call */
#define	EIO		5	/* I/O error */
#define	ENXIO	6	/* No such device or address */
#define	E2BIG	7	/* Argument list too long */
#define	ENOEXEC	8	/* Exec format error */
#define	EBADF	9	/* Bad file number */
#define	ECHILD	10	/* No child processes */
#define	EAGAIN	11	/* Try again */
#define	ENOMEM	12	/* Out of memory */
#define	EACCES	13	/* Permission denied */
#define	EFAULT	14	/* Bad address */
#define	ENOTBLK	15	/* Block device required */
#define	EBUSY	16	/* Device or resource busy */
#define	EEXIST	17	/* File exists */
#define	EXDEV	18	/* Cross-device link */
#define	ENODEV	19	/* No such device */
#define	ENOTDIR	20	/* Not a directory */
#define	EISDIR	21	/* Is a directory */
#define	EINVAL	22	/* Invalid argument */
#define	ENFILE	23	/* File table overflow */
#define	EMFILE	24	/* Too many open files */
#define	ENOTTY	25	/* Not a typewriter */
#define	ETXTBSY	26	/* Text file busy */
#define	EFBIG	27	/* File too large */
#define	ENOSPC	28	/* No space left on device */
#define	ESPIPE	29	/* Illegal seek */
#define	EROFS	30	/* Read-only file system */
#define	EMLINK	31	/* Too many links */
#define	EPIPE	32	/* Broken pipe */
#define	EDOM		33	/* Math argument out of domain of func */
#define 	ERANGE	34	/* Math result not representable */
#define	EALLOC	35 
#define	ENOOBJ 	36
#define	EGOON	37	/* go on */
#define	ECPLT	38	/* Complete */
#define	EPARAM	39	/* 参数错误 */


#endif //__TYPE_T_H