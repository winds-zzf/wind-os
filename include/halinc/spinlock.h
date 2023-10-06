/**
 * CPU控制模块
 * API定义
 */
#ifndef __SPINLOCK_H
#define __SPINLOCK_H


/**
 * 自旋锁初始化
 */
void spinlock_init(Spinlock *lock);

/**
 * 自旋锁上锁
 */
void spinlock_lock(Spinlock *lock);

/**
 * 自旋锁解锁
 */
void spinlock_unlock(Spinlock *lock);

/**
 * 自旋锁上锁并关中断
 */
void spinlock_lock_cli(Spinlock *lock, cpuflag_t *cpuflag);

/**
 * 自旋锁解锁并开中断
 */
void spinlock_unlock_sti(Spinlock *lock, cpuflag_t *cpuflag);


/**
 * 自旋锁上锁，保存cpu寄存器，关中断
 */
void spinlock_lock_saveflg_cli(Spinlock *lock, cpuflag_t *cpuflag);

/**
 * 自旋锁解锁，恢复cpu寄存器，开中断
 */
void spinlock_unlock_restflg_sti(Spinlock *lock, cpuflag_t *cpuflag);


#endif //__SPINLOCK_H