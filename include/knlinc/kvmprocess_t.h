/**
 *
 */
#ifndef __KVMPROCESS_T_H
#define __KVMPROCESS_T_H

#include "mmu_t.h"

/* 进程占用内存空间管理 */
typedef struct  KVMPROCESS{
	spinlock_t	lock;	//保护自身结构的自旋锁
	list_t		hook;	//链表钩子
	
	uint_t 	flag;	//相关标志
	uint_t	status;	//自身状态
	uint_t	count;	//共享计数
	
	sem_t	sem;		//信号量
	mmu_t	mmu;		//MMU相关描述符(类型待定，非指针)
	KvmSpace 	kvmSpace;	//虚拟内存空间（非指针）

	addr_t	stext;	//应用指令区的开始、结束地址
	addr_t	etext;
	addr_t	sdata;	//应用数据区的开始、结束地址
	addr_t	edata;
	addr_t	sbss;	//应用存储未初始化全局变量和静态变量的段的起始和结束地址
	addr_t	ebss;
	addr_t	sheap;	//应用堆区的开始、结束地址
	addr_t	eheap;
}KvmProcess;
/* 声明进程虚拟内存空间 */
extern KvmProcess kvmProcess;


#endif //__KVMPROCESS_T_H