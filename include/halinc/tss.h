/**
 * 任务状态段
 * API定义
 */
#ifndef __TSS_H
#define __TSS_H

#include "gdt_t.h"

/**
 * 设置tss结构体字段值
 * segment: 段描述符
 * base: 
 * limit: 
 * attribute: 
 * 
 */
void set_tss(Segment *segment, u64_t base, u32_t limit, u16_t attribute);


#endif //__TSS_H