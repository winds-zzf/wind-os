/**
 * CPU控制模块
 * API定义
 */
#ifndef __CPU_H
#define __CPU_H

#include"global.h"

/**
 * 输出CPU信息
 */
void cpu_display();

/**
 * 初始化CPU
 * Machine:机器信息结构体
 */
void cpu_init(Machine *mach);


#endif //__CPU_H

