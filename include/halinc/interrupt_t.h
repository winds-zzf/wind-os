/**
 * 中断初始化
 * 类型定义
 */

#ifndef __INTERRUPT_T_H
#define __INTERRUPT_T_H

#include "device_t.h"

#define IFT_MAX	256			//中断异常描述符表最大值

/** 
 * 中断异常描述(Interrupt Fault Descriptor)
 */
typedef struct INTFAULT{
	//状态信息
	spinlock_t 	lock;		//保护该结构体的自旋锁
	u32_t 	flag;		//标志字段
	u32_t	status;		//状态字段
	//中断信息
	uint_t	priority;		//中断优先级(有些中断需要紧急执行)
	uint_t	irq;			//中断号
	uint_t	deep;		//中断嵌套深度
	u64_t	count;		//中断计数
	//使用中断回调函数方式
	list_t		servlst;		//中断服务例程链表
	uint_t	servnum;		//中断服务例程个数
	//使用线程方式
	list_t		thdlst;		//中断线程链表头
	uint_t	thdnum;		//中断线程个数
	void*	thd;			//只有一个中断线程时直接使用指针
	void* 	rbtree;		//如果中断线程太多则按优先级组成红黑树
}IntFault;
/* 中断异常描述符表 */
extern IntFault intFaults[IFT_MAX];


/** 
 * 中断入口函数指针：
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
typedef drvstus_t (*IntCallback)(uint_t intnum,device_t* devp,void* sframe);


/**
 * new：中断服务描述符
 * 用于中断回调函数句柄
 */
typedef struct INTSERVICE_T{
	u32_t 		flag;		//标志
	uint_t 		index;		//中断回调函数运行计数
	//1.挂载到中断异常描述符
	list_t 		inthook;		//钩子:挂载到中断异常描述符
	IntFault* 	ifault;		//指向挂载自己的中断异常描述符
	//2.挂载到设备描述符
	list_t 		devhook;		//钩子:挂载到设备描述符
	void* 		dev;			//指向挂载自己的设备描述符
	//
	IntCallback 	entry;		//中断处理的回调函数指针（handle）
}intservice_t;


# endif //__INTERRUPT_T_H