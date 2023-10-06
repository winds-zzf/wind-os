/**
 * 等待队列
 * API定义
 */
#ifndef __WAITLIST_H
#define __WAITLIST_H

/**
 *
 */
void waitlist_t_init(waitlist_t* wlst);

/**
 *
 */
void waitlist_wait(waitlist_t* wlst);

/**
 *
 */
void waitlist_up(waitlist_t* wlst);

/**
 *
 */
void waitlist_allup();

/**
 * 将线程添加到等待队列上
 */
void waitlist_add(waitlist_t *wlst,thread_t* thread);

/**
 * 
 * 从等待队列中移除线程
 */
thread_t* waitlist_del(waitlist_t* wlst);


#endif //__WAITLIST_H
