/**
 * 进程调度模块
 * API定义
 */
#ifndef __SCHEDULE_H
#define __SCHEDULE_H

/**
 * 初始化全局进程调度器
 */
void schedule_init();

/**
 * 进程调度函数
 */
void krl_schedule();

/**
 * 
 */
void first_sched(thread_t* thread);

/**
 * 向调度器添加一个进程
 */
void schedule_add(thread_t* thread);

/**
 * 调度器休眠当前进程
 * 将当前进程挂载到等待队列
 */
void schedule_wait(waitlist_t* wlst);

/**
 * 调度器唤醒一个进程
 * 将从等待队列中选择一个进程挂载到就绪队列
 */
void schedule_up(waitlist_t* wlst);

/**
 *
 */
void schedule_set_flag();

/**
 *
 */
u64_t schedule_get_flag();

/**
 *
 */
void schedule_set_flag_ex(uint_t flag);

/**
 *
 */
void schedule_inc_tick();

/**
 *
 */
void schedule_check();


#endif //__SCHEDULE_H