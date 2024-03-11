/**
 * 自旋锁结构
 * 类型定义
 */
#ifndef __SPINLOCK_T_H
#define __SPINLOCK_T_H

/**
 * 自旋锁主要是为了应对多CPU环境而设计的。
 *
 * 在多CPU环境中，多个CPU核心可能同时访问共享资源，如果没有合适的同步机制，
 * 就会出现竞争条件（race condition）的问题。竞争条件会导致数据不一致或者不正确的结果。
 * 自旋锁是一种基本的同步机制，它使用了一个标志位来表示资源的状态。
 * 当一个CPU想要访问共享资源时，它会先尝试获取自旋锁。
 * 如果锁处于未锁定状态，那么该CPU会将锁的状态设置为锁定，并继续执行。
 * 如果锁已经被其他CPU锁定，那么该CPU会在一个循环中等待，不断检查锁的状态，直到锁被释放。
 * 在多CPU环境中，自旋锁的等待过程是一个自旋的过程，即CPU会不断地检查锁的状态，而不是进入睡眠状态等待。
 * 这样可以减少上下文切换的开销，并提高并发性能。
 *
 * 因此，自旋锁主要用于多CPU环境中，以确保对共享资源的访问是互斥的，并防止竞争条件的发生。
 * 在单CPU环境中，自旋锁的效果相对较小，因为只有一个CPU在执行代码。
 */

/* 自旋锁结构体 */
typedef struct SPINLOCK_T{
	volatile u32_t lock;	//自旋锁
}spinlock_t;

#endif //__SPINLOCK_T_H