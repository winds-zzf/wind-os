/**
 * 中断初始化
 * API定义
 */

#ifndef __INTERRUPT_H
#define __INTERRUPT_H


#include "interrupt_t.h"

/**
 * 初始化中断控制
 * mach: 机器信息结构体
 */
void interrupt_init();

/**
 * 显示中断控制信息
 * mach: 机器信息结构体
 */
void interrupt_display();

/**
 * 初始化中断服务例程，并将指定字段置位
 * ihandle:中断回调函数句柄 
 * flag: 标志位
 * ifault: 中断异常描述符
 * device: 设备描述符
 * entry: 回调函数入口
 */
void intservice_t_init(intservice_t *intserv,u32_t flag,IntFault *ifault,void* device,IntCallback entry);

/**
 *
 */
bool_t add_intservice(IntFault *ifault,intservice_t* iservice);

/**
 * 根据中断号获取中断异常描述符
 */
IntFault* get_intFault(uint_t irq_num);

/**
 * 异常分发器：用于处理异常中断
 * int_num: 中断号
 * sframe: 栈帧指针 
 */
void hal_fault_allocator(uint_t fault_num,void *sframe);


/**
 * 中断处理函数
 * int_num: 中断异常号
 * sframe: 栈帧指针 
 */
void hal_hwint_allocator(uint_t int_num, void *sframe);


/**
 * 系统调用分发器，用于处理系统调用
 * sys_num: 系统调用号
 * msgp: 消息指针
 */
sysstus_t hal_syscal_allocator(uint_t sys_num,void* msgp);


/**
 * 开启指定硬件中断信号线上的中断
 */
drvstus_t hal_enable_intline(u32_t intnum);

/**
 * 屏蔽指定硬件中断信号线上的中断
 */
drvstus_t hal_disable_intline(u32_t intnum);


# endif //__INTERRUPT_H