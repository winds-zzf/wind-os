/**
 * GDT初始化模块
 * API定义
 */

#ifndef __GDT_H
#define __GDT_H


/**
 * 初始化gdt
 */
void gdt_init();

/**
 * 展示gdt信息
 */
void gdt_display();


#endif //__GDT_H