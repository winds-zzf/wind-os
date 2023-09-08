/**
 * e820信息数组 
 * API定义
 */
#ifndef __E820_H
#define __E820_H

#include"global.h"

/**
 * 初始化E820
 * mach: 机器信息结构体
 */
void e820_init(Machine *mach);


/**
 * 输出E820信息
 * mach: 机器信息结构体
 */
void e820_display(Machine mach);

#endif	//__E820_H