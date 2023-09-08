/**
 * 磁盘信息模块
 * API定义
 */
#ifndef __DISK_H
#define __DISK_H

/**
 * 初始化磁盘信息
 * mach:机器信息结构体
 */
void disk_init(Machine *mach);

/**
 * 回显磁盘信息
 * mach:机器信息结构体
 */
void disk_display(Machine mach);

#endif //__DISK_H