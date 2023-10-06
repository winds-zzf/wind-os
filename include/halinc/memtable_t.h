/**
 * 内存区
 * 类型定义
 */

#ifndef __MEMTABLE_T_H
#define __MEMTABLE_T_H


/* uncertain macro definitions */
#define BAFH_STUS_INIT 0		//List类型初始值
#define BAFH_STUS_ONEM 1		//单链表类型one
#define BAFH_STUS_DIVP 2		//链表数组
#define BAFH_STUS_DIVM 3		//链表数组
/* 内存页链表 */
typedef struct MEMLIST{
	Spinlock 	lock;		//保护自身结构的自旋锁
	uint_t	status;		//状态
	uint_t	order;		//链表的阶：阶为2说明这个链表中每个节点挂2^2个page
	size_t	size;		//当前链表每个节点的pages数
	size_t	total_num;	//此内存表的总页面总数
	size_t 	free_num;		//此内存表空闲页面数
	size_t	allocate_num;	//分配计数
	size_t	recycle_num;	//回收计数
	List		free_pages;	//挂载到此结构的空闲page数
	List		allocate_pages;//挂载到此结构的已分配的page数
}MemList;

#define MEMTABLE_RELDIV 0	//实际分配
#define MEMTABLE_MAXDIV 1	//最大分配
#define MEMLIST_MAX 52		//64位地址结构，4KB分页，最多有2^52物理内存页
/* 内存区分配表结构 */
/**
 * 为什么要有这个table结构
 */
typedef struct MEMTABLE{
	Spinlock 	lock;		//保护自身结构的自旋锁
	uint_t	status;		//table状态
	uint_t	allocate_num;	//内存分配次数
	uint_t	recycle_num;	//内存回收次数
	MemList	lists[MEMLIST_MAX];	//多级链表(order)
	MemList	list;		//单个链表
}MemTable;


#endif //__MEMTABLE_T_H