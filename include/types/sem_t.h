/**
 * 信号量semaphore结构
 * 类型定义
 */
#ifndef __SEM_T_H
#define __SEM_T_H

#include "list_t.h"
#include "spinlock_t.h"

/* 信号量结构体 */
typedef struct SEM{
	Spinlock lock;
	uint_t flag;
	sint_t count;
	//阻塞列表;
}Sem;

/*  */
typedef struct s_ATOMIC{
	volatile s32_t a_count;
}atomic_t;

/*  */
typedef struct s_WAIT_L_HEAD{
	List wlh_llist;		//
	Spinlock wlh_lock;		//
	atomic_t wlh_count;		//
	void* wlh_privte;		//
	bool_t  (*wlh_upfun)(u32_t func,struct s_WAIT_L_HEAD* wlhp);	//
}wait_l_head_t;


//信号量和PV操作


#endif //__SEM_T_H