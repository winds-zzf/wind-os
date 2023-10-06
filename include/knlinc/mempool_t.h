/**
 * 内存池模块
 * 类型定义
 */
#ifndef __MEMPOOL_T_H
#define __MEMPOOL_T_H


#define MEMPOOL_APPLY_MAX 0x400000	//最小可分配内存:4MB
#define MEMPOOL_APPLY_MIN 1			//最大可分配内存:1B

#define MEMPOOL_LIMIT 2048	//界定使用内存对象还是内存页的临界值，大于2048分配物理页，否则分配内存对象


#define MEMPOOL_LIST_MAX 10	//最多分配1024个连续内存页

/**
 * 内存池未按照源码上面的定义
 */
/* 定义内存池数据结构 */
typedef struct MEMPOOL_T{
	Spinlock	lock;
	List		hook;
	uint_t	status;
	uint_t	flags;
	//链表，挂载已经分配的内存页
	List		pages[MEMLIST_MAX];
}mempool_t;

/* 声明全局内存池 */
extern mempool_t mempool;


#endif //__MEMPOOL_T_H