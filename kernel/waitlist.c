/**
 * 等待队列结构
 * API实现
 */

#include "globaltype.h"
#include "globalctrl.h"

/**
 * 初始化等待队列结构
 */
void waitlist_t_init(waitlist_t* wlst){
	spinlock_init(&wlst->lock);
	list_init(&wlst->waits);
	wlst->waitsNum = 0;
	
	return;
}


void waitlist_wait(waitlist_t* wlst){
	//调用调度器的休眠函数，让进程休眠，并挂入等待队列wlst
	schedule_wait(wlst);
	return; 
}

void waitlist_up(waitlist_t* wlst){
	if(list_is_empty_careful(&wlst->waits)==FALSE){
		//从wlst等待队列中唤醒一个进程，加入到就绪队列
		schedule_up(wlst);	
	}
	return;
}

/**
 * 将等待队列中的所有进程唤醒，加入到就绪队列等待
 */
void waitlist_allup(waitlist_t* wlst){
	//将等待队列所有的进程都添加到进程调度
	while(list_is_empty_careful(&wlst->waits)==FALSE){
		//选择一个进程，挂载到就绪队列
		schedule_up(wlst);
	}
	return;
}


/**
 * 将线程添加到等待队列上
 */
void waitlist_add(waitlist_t *wlst,thread_t* thd){
	cpuflag_t cpuflag;
	spinlock_lock_cli(&wlst->lock,&cpuflag);
	//先进先出:放在队列尾部
	list_add_tail(&thd->hook,&wlst->waits);
	wlst->waitsNum++;

	spinlock_unlock_sti(&wlst->lock,&cpuflag);
	return;
}

/**
 * 
 * 从等待队列中移除线程
 */
thread_t* waitlist_del(waitlist_t* wlst){
	thread_t* thd=NULL;
	
	cpuflag_t cpuflag;
	spinlock_lock_cli(&wlst->lock,&cpuflag);

	//从等待队列中取出一个线程
	if(wlst->waitsNum>0){
		thd = list_entry(wlst->waits.next,thread_t,hook);
		list_del(&thd->hook);
		wlst->waitsNum--;
	}

	spinlock_unlock_sti(&wlst->lock,&cpuflag);

	return thd;
}

