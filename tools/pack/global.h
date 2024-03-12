#ifndef __GLOBAL_H
#define __GLOBAL_H

#define OK 1
#define ERROR 0;
typedef int Status;

#define ALIGN(x,b) (((x)+(b)-1)&~((b)-1)) //对齐
#define ALIGN_4K(x) ALIGN((x),0x1000)	//4K对齐

typedef unsigned int u32_t;
typedef unsigned long long u62_t;

#endif //__GLOBAL_H