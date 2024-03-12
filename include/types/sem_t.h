/**
 * 信号量semaphore结构
 * 类型定义
 */
#ifndef __SEM_T_H
#define __SEM_T_H


#include "list_t.h"
#include "spinlock_t.h"


#define SEM_FLAG_MUTEX 0
#define SEM_FLAG_MULTI 1


#define SEM_MUTEX_LOCK 1		//当做互斥信号量使用
#define SEM_MULTI_LOCK 0		//


/* 等待队列 */
typedef struct WAITLIST_T{
	spinlock_t 	lock;		//保护此结构的自旋锁
	list_t		waits;		//等待进程队列
	size_t		waitsNum;		//等待队列长度
}waitlist_t;


/* 信号量结构体 */
typedef struct SEM_T{
	spinlock_t 	lock;	//保护信号量的自旋锁
	uint_t 		flag;	//信号量标志
	sint_t 		count;	//资源计数
	waitlist_t	wlst;	//该信号量所描述资源的等待队列
}sem_t;


#endif //__SEM_T_H