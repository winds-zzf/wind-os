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

//专用类型
typedef unsigned int uint_t;	//无符号int
typedef signed int sint_t;	//有符号int
typedef u64_t boolean_t;		//使用u64_t以提供最大的兼容性
typedef u64_t addr_t;		//地址类型
typedef u64_t size_t;		//大小类型
typedef char char_t;		//字符类型
typedef u8_t byte_t;		//子节类型

//硬件类型
typedef u64_t eflags;	//CPU标志寄存器

//宏定义
#define public			//公有域：default
#define private static 	//私有域
#define EXTERN extern	//extern
#define INLINE static inline 		//静态内置函数
#define NULL 0			//空指针
#define TRUE 1			//true
#define FALSE 0		//falze

#define ALIGN(x,b) (((x)+(b)-1)&~((b)-1))	//x按照b的倍数向上取整

#define HALT()	//主动死机


#endif //__TYPE_T_H