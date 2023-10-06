/**
 * 内存区
 * 类型定义
 */


#ifndef __MEMAREA_T_H
#define __MEMAREA_T_H


#include "memtable_t.h"

#define MEMAREA_TYPE_INIT 0	//初始值(用于初始化)
#define MEMAREA_TYPE_HWAD 1	//内存硬件区(HArDWare)
#define MEMAREA_TYPE_KRNL 2	//内存内核区(KeRNeL)
#define MEMAREA_TYPE_APPS 3	//内存应用区(APPlicationS)
#define MEMAREA_TYPE_SHAR 4	//内存共享区(SHARe)
/** 
 * 硬件区
 * 低32MB([0x0,0x2000000])内存区域通常保留给硬件区使用，这是因为早期计算机内存较小，只能访存32MB。
 * 如MMU页表、BIOS中断表等硬件信息
 */
#define MEMAREA_HWAD_START 0
#define MEMAREA_HWAD_SIZE 0x2000000
#define MEMAREA_HWAD_END (MEMAREA_HWAD_START+MEMAREA_HWAD_SIZE-1)
/* 内核区 */
#define MEMAREA_KRNL_START 0x2000000
#define MEMAREA_KRNL_SIZE (0x40000000-0x2000000)
#define MEMAREA_KRNL_END (MEMAREA_KRNL_START+MEMAREA_KRNL_SIZE-1)
/* 应用区 */
#define MEMAREA_APPS_START 0x40000000
#define MEMAREA_APPS_SIZE (0xffffffffffffffff-0x40000000)
#define MEMAREA_APPS_END (MEMAREA_APPS_START+MEMAREA_APPS_SIZE-1)
/* 共享区 */

#define MEMAREA_MAX 4	//最大内存区数量
/* 内存区数据结构 */
typedef struct MEMAREA{
	List 	list;		//链表
	Spinlock 	lock;		//自旋锁
	
	uint_t 	status;		//内存区状态
	uint_t	flag;		//内存区标志
	uint_t	type;		//内存区类型
	
	//暂未实现
	sem_t	sem;			//内存区信号量
	;					//内存区等待队列
	
	size_t	total_num;	//内存区总页面数
	size_t	allocate_num;	//内存区已分配页面数
	size_t	free_num;		//内存区空闲页面数
	size_t	retain_num;	//内存区保留页面数
	size_t	horizon;		//内存分配的水位线
	
	addr_t	start;		//内存区开始地址
	addr_t	end;			//内存区结束地址
	size_t	size;		//内存区大小
	
	MemTable	table;		//内存分配表
}MemArea;


#endif //__MEMAREA_T_H