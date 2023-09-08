/**
 * CPU控制模块
 * API定义
 */
#ifndef __CPUCTRL_H
#define __CPUCTRL_H

/**
 * 初始化自旋锁
 * lock: 自旋锁
 */
void hal_spinlock_init(Spinlock *lock);

/**
 * 获取自旋锁
 * lock: 自旋锁
 */
void hal_spinlock_lock(Spinlock *lock);

/**
 * 释放自旋锁
 * lock: 自旋锁
 */
void hal_spinlock_unlock(Spinlock *lock);


/**
 * 获取自旋锁并保存当前CPU标志寄存器的值，并禁用中断
 * lock: 自旋锁
 * cpuflag: cpu标志寄存器
 */
void hal_spinlock_saveflg_cli(Spinlock *lock,cpuflag_t *cpuflag);

/**
 * 释放自旋锁并恢复之前保存的CPU标志寄存器的值，并启用中断
 * lock: 自旋锁
 * cpuflag: cpu标志寄存器
 */
void hal_spinunlock_restflg_sti(Spinlock *lock,cpuflag_t *cpuflag);

/**
 *
 */
void knl_spinlock_init(Spinlock *lock);

/**
 *
 */
void knl_spinlock_lock(Spinlock *lock);

/**
 *
 */
void knl_spinlock_unlock(Spinlock *lock);

/**
 *
 */
void knl_spinlock_cli(Spinlock *lock, cpuflag_t *cpuflag);

/**
 *
 */
void knl_spinunlock_sti(Spinlock *lock, cpuflag_t *cpuflag);


#endif //__CPUCTRL_H