/**
 * 字符串处理
 * 类型定义
 */
#ifndef __STDIO_T_H
#define __STDIO_T_H


#define BUFFER_SIZE	1024U 	//字符缓冲区大小

typedef __builtin_va_list va_list;		//使用GCC内置的可变参数列表作为自己的可变参数列表类型

#define va_start(ap,np) __builtin_va_start(ap,np)

#define va_arg(ap,ty)	__builtin_va_arg(ap,ty)

#define va_end(ap)	__builtin_va_end(ap)


#endif //__STDIO_T_H
