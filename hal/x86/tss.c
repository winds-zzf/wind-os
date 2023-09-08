/**
 * 任务状态段
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

/**
 * 
 */
void set_tss(Segment *segment, u64_t base, u32_t limit, u16_t attribute){
	u32_t *x64tssb_h = (u32_t *)(segment + 1);
	segment->limit_low = limit & 0x0FFFF;		// 段界限 1(2 字节)
	segment->base_low = base & 0x0FFFF;		// 段基址 1(2 字节)
	segment->base_mid = (base >> 16) & 0x0FF;	// 段基址 2(1 字节)
	segment->attr1 = (u8_t)(attribute & 0xFF);	// 属性 1
	segment->limit_high_attr2 = (u8_t)(((limit >> 16) & 0x0F) | ((attribute >> 8) & 0xF0));// 段界限 2 + 属性 2
	segment->base_high = (u8_t)((base >> 24) & 0x0FF);
	*x64tssb_h = (u32_t)((base >> 32) & 0xffffffff);
	*(x64tssb_h + 1) = 0;
	
	return ;
}