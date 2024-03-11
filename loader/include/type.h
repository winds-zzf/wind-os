/**
 * 二级引导器全局类型头文件
 */

#ifndef _TYPE_H
#define _TYPE_H

//定义无符号整数
typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;
typedef unsigned long long u64_t; //Linux 32bits mode

//定义有符号整数
typedef signed char s8_t;
typedef signed short s16_t;
typedef signed int s32_t;
typedef signed long long s64_t;

//定义int
typedef signed int sint_t;        //定义32bits有符号int
typedef unsigned int uint_t;      //定义32bits无符号int
typedef u8_t byte_t;              //定义子节类型

typedef u64_t eflags_t;           //cpu标志寄存器类型

typedef u32_t addr_t;             //定义地址类型4B（与指针长度相同）
typedef u32_t size_t;             //定义大小类型

typedef u64_t bool_t;          //定义布尔类型
typedef char* str_t;			//定义字符串类型
typedef char char_t;              //定义字符类型

#define NULL 0      //空指针
#define TRUE 1      //boolean:true
#define FALSE 0     //boolean:false 
#define INLINE static inline      //定义静态内置函数

#define ALIGN(x,b)  (((x)+(b)-1)&~((b)-1))  //x向上x对齐
#define ALIGN_4K(x) ALIGN(x,0x1000)		//x向上4K对齐
#define HALT() __asm__ __volatile__("cli; hlt": : :"memory")	//主动死机


#endif //_TYPE_H