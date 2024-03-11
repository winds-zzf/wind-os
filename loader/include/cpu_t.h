/**
 * CPU控制模块
 * 类型定义
 */
#ifndef __CPU_T_H
#define __CPU_T_H


#define HALT() __asm__ __volatile__("cli; hlt": : :"memory")	//主动死机: 禁用中断，并使处理器进入停机状态，它会使处理器停止执行指令


#endif //__CPU_T_H
