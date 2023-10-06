/**
 * 原子操作
 * 类型定义
 */
#ifndef __ATOMIC_T_H
#define __ATOMIC_T_H


/** 
 * automic type
 * 原子类型，提供原子操作的功能
 */
typedef struct s_ATOMIC_T{
	//避免编译器优化，每次都是从内存中直接访问变量，而不通过缓存
	volatile s32_t value;
}atomic_t;


/* 通过原子类型实现的引用计数 */
typedef struct REFCOUNT_T{
	atomic_t ref_count;
}refcount_t;


#endif //__ATOMIC_T_H