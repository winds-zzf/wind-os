/**
 * 信号量结构
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

void sem_t_init(sem_t* sem){
	spinlock_init(&sem->lock);
	sem->flag = 0;
	sem->count = 0;
	waitlist_t_init(&sem->wlst);
	return;
}

/**
 * 设置信号量的值
 * 信号量的值描述了同种资源的数量，如果计数为0，此时不再减少，也可以改为负值
 */
void sem_set(sem_t* sem,uint_t flag,uint_t count){
	cpuflag_t cpuflag;
	spinlock_lock_cli(&sem->lock,&cpuflag);

	sem->flag = flag;
	sem->count = count;
	
	spinlock_unlock_sti(&sem->lock,&cpuflag);
	return;
}

/**
 * 信号量P原语
 * 
 * 尝试申请sem对应的资源，申请成功返回后继续执行
 * 如果申请失败，将进程加入到该资源的等待队列，并进行进程调度
 */
void sem_down(sem_t* sem){
	cpuflag_t cpuflag;
start_step:
	spinlock_lock_cli(&sem->lock,&cpuflag);
	//当信号量的值小于1时，表示资源不够分配，测试阻塞进程
	if(sem->count<1){
		//当前进程进入等待队列
		schedule_wait(&sem->wlst);
		spinlock_unlock_sti(&sem->lock,&cpuflag);

		//当前进程阻塞，重新调度进程进行进程切换
		krl_schedule();
		goto start_step; //当前进程被调度后，执行goto，继续执行P原语
	}

	//信号量值大于等于1，分配资源
	sem->count--;
	spinlock_unlock_sti(&sem->lock,&cpuflag);
	return;
}

/**
 * 信号量V原语
 * 在进入sem_up时要立刻上锁并关中断，避免其他进程同时访问sem_up函数导致局部变量被修改
 */
void sem_up(sem_t* sem){
	cpuflag_t cpuflag;
	
	spinlock_lock_cli(&sem->lock,&cpuflag);
	sem->count++;
	//当资源不够时，count并没有继续减一，而是维持0
	if(sem->count<1){
		spinlock_unlock_sti(&sem->lock,&cpuflag);
		printk("error in sem_up\n");die(0);
	}
	
	/**
	 * 唤醒所有等待该资源进程，加入到等待队列，重新等待进程调度
	 */
	waitlist_allup(&sem->wlst);
	spinlock_unlock_sti(&sem->lock,&cpuflag);	
	
	/**
	 * 将调度器标志改为“需要调度”，告诉调度器现在需要调度
	 * 
	 * 这里只是标记为需要调度，因为我要等到当前未阻塞的进程运行结束
	 * 如果使用krl_schedule()那直接抢占式调度了
	 */
	schedule_set_flag();
	
	return;
}

