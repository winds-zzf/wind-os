/**
 * GDT初始化模块
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"


/**
 * 加载GDT表信息到gdtr寄存器
 */
static void load_gdt(){
	__asm__ __volatile__(
		"cli \n\t"
		"pushq %%rax \n\t"
		"lgdt (%0) \n\t"
		"movabsq $1f,%%rax \n\t"
		"pushq   $8 \n\t"
		"pushq   %%rax    \n\t"
		"lretq \n\t"
		"1:\n\t"
		"movw $0x10,%%ax\n\t"
		"movw %%ax,%%ds\n\t"
		"movw %%ax,%%es\n\t"
		"movw %%ax,%%ss\n\t"
		"movw %%ax,%%fs\n\t"
		"movw %%ax,%%gs\n\t"
		"popq %%rax \n\t"
		:
		: "r"(&gdtr)
		: "rax", "memory"
		);
	return;
}

/**
 * 加载任务寄存器的值
 * TR是任务寄存器(Task Register)的缩写
 * trindx:tr index
 */
static void load_tr(u16_t trindx){
	__asm__ __volatile__(
		"ltr %0 \n\t"
		:
		: "r"(trindx)
		: "memory");
	return;
}

/**
 * 
 * segment: 
 * base: 
 * limit: 
 * attribute: 
 * 
 */
static void set_segment(Segment *segment, u32_t base, u32_t limit, u16_t attribute){
    segment->limit_low = limit & 0x0FFFF;		// 段界限 1(2 字节)
    segment->base_low = base & 0x0FFFF;			// 段基址 1(2 字节)
    segment->base_mid = (base >> 16) & 0x0FF;	// 段基址 2(1 字节)
    segment->attr1 = (u8_t)(attribute & 0xFF);	// 属性 1
    segment->limit_high_attr2 = (u8_t)(((limit >> 16) & 0x0F)|((attribute >> 8)&0xF0)); // 段界限2 + 属性2
    segment->base_high = (u8_t)((base >> 24) & 0x0FF);  // 段基址 3\(1 字节)
    return;
}

/**
 * 初始化gdt
 */
void gdt_init(){
	for (u32_t i = 0; i < CPUCORE_MAX; i++){
		/**
		 * 为什么这样设置？？？
		 */
		set_segment(&gdt[i][0], 0, 0, 0);	//空白描述符：用于占位
		set_segment(&gdt[i][1], 0, 0, DA_CR | DA_64 | 0);
		set_segment(&gdt[i][2], 0, 0, DA_DRW | DA_64 | 0);
		set_segment(&gdt[i][3], 0, 0, DA_CR | DA_64 | DA_DPL3 | 0); 
		set_segment(&gdt[i][4], 0, 0, DA_DRW | DA_64 | DA_DPL3 | 0);
		//设置tss
		set_tss(&gdt[i][6], (u64_t)&x64tss[i], sizeof(x64tss[i]) - 1, DA_386TSS);
		gdtr[i].base = (u64_t)gdt[i];
		gdtr[i].len = sizeof(gdt[i]) - 1;
	}
	
	//设置并加载GDTR
	load_gdt();
	load_tr(0x30);
	
	return;
}