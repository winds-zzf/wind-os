/**
 * 内存管理
 * 类型定义
 */
#ifndef __MEMMGR_T_H
#define __MEMMGR_T_H


#include "memview_t.h"
#include "mempage_t.h"
#include "memarea_t.h"
#include "memobject_t.h"

/** 
 * 内存管理结构体
 * 在完成内存初始化之后，就不再使用物理地址了，这也正是hal层进行抽象的目的之一
 * 在hal层把和硬件相关的东西用结构体或函数封装起来，上层通过hal来访问使用硬件。
 */
typedef struct MEMMGR{
	//
	List		list;		//内存管理结构自身的链表
	Spinlock	lock;		//保护自身的自旋锁
	//
	uint_t	status;		//状态
	uint_t	flag;		//标志
	//
	u64_t	memsize;		//分页情况下的内存大小
	u64_t	max_pages;	//内存最大页面数
	u64_t	free_pages;	//内存最大空闲页面数
	u64_t	allocate_pages;//内存最大分配页面数
	u64_t	retain_pages;	//内存保留页面数
	u64_t	horizon;		//内存分配水位线
	//
	MemView* 	views;		//内存空间布局结构指针
	u64_t	views_num;	//内存空间布局结构数量
	MemPage*	pages;		//内存页表地址
	u64_t	pages_num;	//内存页表数量 
	MemArea*	areas;		//内存区数组
	u64_t	areas_num;	//内存区数量
	
	MObjManager objManager; //内存对象管理
}Memmgr;

/* 内存管理对象 */
extern Memmgr memmgr;


#endif //__MEMMGR_T_H