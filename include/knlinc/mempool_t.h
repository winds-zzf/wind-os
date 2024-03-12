/**
 * 内存池模块
 * 类型定义
 */
#ifndef __MEMPOOL_T_H
#define __MEMPOOL_T_H


/**
 * 最大可分配连续物理页数关于2的阶
 */
#define MEMPOOL_ORDER_MAX  10	

#define MEMPOOL_APPLY_MIN 1								//最小可分配内存:1B
#define MEMPOOL_APPLY_MAX (0x1000*(0x1<<MEMPOOL_ORDER_MAX))	//最大可分配内存:0x1000*(2^10)=4MB

/**
 * [2^0,2^MEMPOOL_ORDER_MAX]共(1+MEMPOOL_ORDER_MAX)个
 */
#define MEMPOOL_LIST_MAX (MEMPOOL_ORDER_MAX+1)

/**
 * 界定使用内存对象还是内存页的临界值，大于2048分配物理页，否则分配内存对象
 */
#define MEMPOOL_LIMIT 2048


/**
 * 内存池未按照源码上面的定义
 */
/* 定义内存池数据结构 */
typedef struct MEMPOOL_T{
	spinlock_t	lock;
	list_t		hook;
	uint_t	status;
	uint_t	flags;
	//链表，挂载已经分配的内存页
	list_t		pages[MEMPOOL_LIST_MAX];
}mempool_t;

/* 声明全局内存池 */
extern mempool_t mempool;


#endif //__MEMPOOL_T_H