/**
 * MMU页表
 * API定义
 */

#ifndef __REMMU_H
#define __REMMU_H


/**
 * 初始化MMU
 * Machine:机器信息结构体
 */
void remmu_init();

/**
 * 输出MMU信息
 * Machine:机器信息结构体
 */
void remmu_display();


#endif //__REMMU_H
