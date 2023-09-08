/**
 * ACPI高级电源配置与管理模块
 * API定义
 */
#ifndef __ACPI_H
#define __ACPI_H

#include"global.h"
#include"acpi_t.h"

/**
 * 初始化ACPI
 * mach: 机器信息结构体
 */
void acpi_init(Machine *mach);


/**
 * 输出ACPI信息
 * mach: 机器信息结构体
 */
void acpi_display(Machine mach);


#endif	//__ACPI_H
