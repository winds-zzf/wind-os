/**
 * 内存页结构
 * 类型定义
 */

#ifndef __MEMPAGE_T_H
#define __MEMPAGE_T_H


#define  PAGEADDR_NO_ALLOC (0)
#define  PAGEADDR_ALLOC (1)
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


/* 标志位 */
#define PAGEFLAGS_MRV1_VAL (0)		//

#define PAGEFLAGS_OLKTY_INIT (0)		//挂入链表类型:初始值
#define PAGEFLAGS_OLKTY_ODER (1)		//挂入链表类型：OrDER：表示页面所在链表为单页面节点链表
#define PAGEFLAGS_OLKTY_BAFH (2)		//挂入链表类型：bafh：表示页面所在链表为多页面节点链表
#define PAGEFLAGS_OLKTY_TOBJ (3)		//挂入链表类型：tobj：表示当前所在链表为内存对象链表

#define PAGEFLAGS_LSTTY_LIST (0)		//是否挂入链表：FALSE

#define PAGEFLAGS_MOCTY_FREE (0)		//分配类型：空闲未分配
#define PAGEFLAGS_MOCTY_KRNL (1)		//分配类型：内核
#define PAGEFLAGS_MOCTY_USER (2)		//分配类型：应用

#define PAGEFLAGS_UINDX_INIT (0x0)	//分配计数：初始值
#define PAGEFLAGS_UINDX_MAX (0xffffff)	//分配计数：最大值

/* 暂未使用 */
#define PAGEFLAGS_MARTY_INIT (0)		//所属内存区：初始值
#define PAGEFLAGS_MARTY_HWAD (1)		//所属内存区：硬件区
#define PAGEFLAGS_MARTY_KRNL (2)		//所属内存区：内核区
#define PAGEFLAGS_MARTY_APPS (3)		//所属内存区：应用区
#define PAGEFLAGS_MARTY_SHAR (4)		//所属内存区：共享区
/* 内存页标志 */
typedef struct PAGEFLAGS{
	u32_t 	olkty:2;		//挂入链表的类型(list_type)
	u32_t 	lstty:1;		//是否挂入链表(is_hook)
	u32_t 	mocty:2;		//分配类型：内核还是应用或者空闲(allocate_type)
	u32_t 	marty:3;		//属于哪个区(affiliate_area)
	u32_t 	count:24;		//分配计数(count)
}PageFlags;

/**
 * 当页尚未分配时：分配类型:空闲 && 分配计数:0 && 地址分配:0
 */

/** 
 * 页描述符
 * 页描述符要尽可能的小，因为物理内存能划分成大量的页
 */
typedef struct MEMPAGE{
	List 	hook;		//16B链表
	Spinlock	lock;		//4B保护自身的自旋锁
	PageFlags flags;		//4B物理页标志
	PageAddr 	addr;		//8B物理地址和标志
	void* 	next;		//8B下一个相邻页(该字段有什么用？)
}MemPage;	//40B


#endif //__MEMPAGE_T_H