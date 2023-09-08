/**
 * 中断初始化
 * 类型定义
 */

#ifndef __INTERRUPT_T_H
#define __INTERRUPT_T_H


#define IFTMAX	256			//中断异常描述符表最大值

/** 
 * 中断异常描述(Interrupt Fault Descriptor)
 */
typedef struct INTFAULT{
	//状态信息
	Spinlock 	lock;		//保护该结构体的自旋锁
	u32_t 	flag;		//标志字段
	u32_t	status;		//状态字段
	//中断信息
	uint_t	priority;		//中断优先级(有些中断需要紧急执行)
	uint_t	irq;			//中断号
	uint_t	deep;		//中断嵌套深度
	u64_t	count;		//中断计数
	//使用中断回调函数方式
	List		callback_list;	//中断回调函数链表
	uint_t	callback_num;	//中断回调函数的个数
	//使用线程方式
	List		thread_list;	//中断线程链表头
	uint_t	thread_num;	//中断线程个数
	void*	thread;		//只有一个中断线程时直接使用指针
	void* 	rbtree;		//如果中断线程太多则按优先级组成红黑树
}IntFault;
/* 中断异常描述符表 */
extern IntFault intFaults[IFTMAX];


/** 
 * 中断处理函数指针：
 * CPU现场保护->执行中断回调函数->CPU现场恢复
 */
typedef void (*IntDispose)();

/** 
 * 中断回调函数指针
 * ifault_num:中断异常号 
 * device:
 * sframe:栈帧指针
 * drvstus_t: 
 * 执行实际的中断处理逻辑
 */
typedef drvstus_t (*IntCallback)(uint_t ifault_num,void* device,void* sframe);


/**
 * 用于中断回调函数句柄
 */
typedef struct INTHANDLE{
	//
	u32_t 		flag;		//
	uint_t 		index;		//
	//
	List 		int_list;		//在中断异常描述符中的链表
	IntFault* 	intFault;		//指向中断异常描述符
	//
	List 		device_list;	//在设备描述符中的链表
	void* 		device;		//指向设备描述符
	//
	IntCallback 	entry;		//中断处理的回调函数入口
}IntHandle;


# endif //__INTERRUPT_T_H