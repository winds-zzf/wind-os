/**
 * 内存页结构
 * 类型定义
 */

#ifndef __MEMPAGE_T_H
#define __MEMPAGE_T_H


/* 页物理地址(高位和标志位) */
typedef struct PAGEADDR{
	u64_t	allocate:1;	//分配位
	u64_t	share:1;		//共享位
	u64_t	swap:1;		//交换位
	u64_t	cache:1;		//缓存位
	u64_t	map:1;		//映射位
	u64_t	dirty:1;		//脏位
	u64_t	busy:1;		//忙位
	u64_t	lock:1;		//锁定位
	u64_t	retain:4;		//保留位
	u64_t	value:52;		//页号
}__attribute__((packed)) PageAddr;


/* 页描述符 */
typedef struct PAGE{
	List 	list;		//链表
	Spinlock	lock;		//保护自身的自旋锁
	
	struct{
		u32_t 	olkty:2;	//挂入链表的类型
		u32_t 	lstty:1;	//是否挂入链表
		u32_t 	mocty:2;	//分配类型，是被谁占用了，内核还是应用或者空闲
		u32_t 	marty:3;	//属于哪个区
		u32_t 	uindx:24;	//分配计数
	}flags;				//内存空间地址描述符标志	
	
	PageAddr 	addr;		//物理地址和标志
	struct PAGE* 	next;	//下一个相邻页
}__attribute__((packed))Page;	//40B


#endif //__MEMPAGE_T_H