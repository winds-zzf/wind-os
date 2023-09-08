/**
 * CPU控制模块
 * API定义
 */
#ifndef __CPU_H
#define __CPU_H

#include"global.h"

/**
 * 初始化CPU
 * mach: 机器信息结构体
 */
void cpu_init(Machine *mach);

/**
 * 输出CPU信息
 * mach: 机器信息结构体
 */
void cpu_display(Machine mach);

#endif //__CPU_H

