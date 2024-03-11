/**
 * 显卡图形模式
 * API定义
 */
#ifndef __GRAPH_H
#define __GRAPH_H

/**
 * 初始化graph
 * mach: 机器信息结构体
 */
void graph_init(Machine *mach);

/**
 * 输出graph信息
 * mach: 机器信息结构体
 */
void graph_display(Machine mach);

#endif	//__GRAPH_H
