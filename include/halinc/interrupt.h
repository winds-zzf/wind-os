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
 * 初始化中断回调函数句柄
 * ihandle:中断回调函数句柄 
 * flag: 标志位
 * ifault: 中断异常描述符
 * device: 设备描述符
 * entry: 回调函数入口
 */
void init_intHandle(IntHandle *ihandle,u32_t flag,IntFault *ifault,void* device,IntCallback entry);

/**
 * 为中断异常描述符添加中断回调函数
 * ifault: 中断异常描述符
 * ihandle: 中断回调函数句柄
 * return 返回true当且仅当添加成功
 */
bool_t  add_ihandle(IntFault *ifault,IntHandle *ihandle);

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

# endif //__INTERRUPT_H