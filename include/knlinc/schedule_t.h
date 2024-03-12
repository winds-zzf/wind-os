/**
 * 进程调度模块
 * 类型定义
 */
#ifndef __SCHEDULE_T_H
#define __SCHEDULE_T_H


/* 调度器调度判断标志 */
#define NOTS_SCHED_FLGS (0)	//无需调度，比如空转进程正在运行，就无需调度
#define NEED_SCHED_FLGS (1)	//需要调度标志，比如时间片用完，进程休眠
#define NEED_START_CPUIDLE_SCHED_FLGS (2)	//需要启动空转进程，用于首次使用调度器时启动空转进程
#define PMPT_FLGS 0		//抢占计数

/* 
 * 优先队列
 */
typedef struct THREADLIST_T{
	list_t 	threads;		//优先队列的链表头
	uint_t 	threadsNum;	//优先队列上的进程数
	/**
	 * 为什么优先队列上也要有current指针？？？
	 */
	thread_t*	curthd;		//优先队列上正在运行的进程
}thdlst_t;


/**
 * CPU进程调度器
 * CPU也是一种资源，这种资源由调度器管理，每个CPU都拥有一个进程调度器管理。
 * “当前进程”表该调度器管理的CPU上当前运行的进程
 */
typedef struct SCHDATA_T{
	spinlock_t		lock;		//保护调度器的自旋锁
	u64_t		cpuid;		//调度器对应CPU的id
	u64_t		flags;		//调度器标志
	u64_t		count;		//进程抢占计数
	u64_t		threadNum;	//调度器的总进程数
	u64_t		priority;		//调度器当前进程的优先级
	thread_t*		cpuidle;		//调度器空转进程
	thread_t*		curthd;		//调度器正在运行的进程
	thdlst_t		thdlst[THREAD_PRIORITY_MAX];	//该调度器的多级优先队列
}schedata_t;


/* 全局调度器类 */
typedef struct SCHDCLASS_T{
	spinlock_t		lock;
	uint_t		cpuNum;		//CPU总数量
	uint_t		thdNum;		//进程总数量
	u64_t		threadid_inc;	//分配进程id所用
	schedata_t	schedatas[CPUCORE_MAX];	//每个CPU都有一个进程调度器		
}scheclass_t;

/*  声明全局调度器 */
extern scheclass_t scheclass;


#endif //__SCHEDULE_T_H