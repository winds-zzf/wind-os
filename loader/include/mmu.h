/**
 * MMU页表
 * API定义
 */

#ifndef __MMU_H
#define __MMU_H

#include"global.h"

/**
 * 初始化MMU
 * Machine:机器信息结构体
 */
void mmu_init(Machine *mach);

/**
 * 输出MMU信息
 * Machine:机器信息结构体
 */
void mmu_display(Machine mach);


#endif //__MMU_H
