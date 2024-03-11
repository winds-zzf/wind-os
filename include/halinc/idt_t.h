/**
 * IDT初始化模块
 * 类型定义
 * x86CPU 长模式
 */
#ifndef __IDT_T_H
#define __IDT_T_H


#define INTMAX 256		//x86 CPU最多支持256个中断

/* 中断向量：interrupt vector */
#define	INT_VECTOR_DIVIDE			0x0
#define	INT_VECTOR_DEBUG			0x1
#define	INT_VECTOR_NMI				0x2
#define	INT_VECTOR_BREAKPOINT		0x3
#define	INT_VECTOR_OVERFLOW			0x4
#define	INT_VECTOR_BOUNDS			0x5
#define	INT_VECTOR_INVAL_OP			0x6
#define	INT_VECTOR_COPROC_NOT		0x7
#define	INT_VECTOR_DOUBLE_FAULT		0x8
#define	INT_VECTOR_COPROC_SEG		0x9
#define	INT_VECTOR_INVAL_TSS		0xA
#define	INT_VECTOR_SEG_NOT			0xB
#define	INT_VECTOR_STACK_FAULT		0xC
#define	INT_VECTOR_PROTECTION		0xD
#define	INT_VECTOR_PAGE_FAULT		0xE
//...0xF号中断未定义
#define	INT_VECTOR_COPROC_ERR		0x10
#define	INT_VECTOR_ALIGN_CHEK		0x11
#define	INT_VECTOR_MACHI_CHEK		0x12
#define	INT_VECTOR_SIMD_FAULT		0x13
//...前32[0-31]号中断是系统保留的中断
#define	INT_VECTOR_IRQ				0x20
//...
#define	INT_VECTOR_APIC_IPI_SCHEDUL	0xe0
#define	INT_VECTOR_APIC_SVR			0xef
#define	INT_VECTOR_APIC_TIMER		0xf0
#define	INT_VECTOR_APIC_THERMAL		0xf1
#define	INT_VECTOR_APIC_PERFORM		0xf2
#define	INT_VECTOR_APIC_LINTO		0xf3
#define	INT_VECTOR_APIC_LINTI		0xf4
#define	INT_VECTOR_APIC_ERROR		0xf5
//...
#define	INT_VECTOR_SYSCALL			0xFF

/**
 * 中断门描述符
 * 固定结构：中断门结构体是按照x86CPU长模式下的中断门描述符
 * 逆序定义：GCC默认小端模式
 * 禁用对齐：因为每个字段偏移是固定的
 */
typedef struct INTGATE{
	u16_t 	offset_low;		//目标代码段偏移(15:0)
	u16_t 	selector;			//目标代码段选择子
	u8_t 	dcount;			//保留+IST
	u8_t 	attr;			//P(1) DPL(2) DT(1) TYPE(4)
	u16_t 	offset_mid;		//目标代码段偏移(31:16)
	u32_t 	offset_high;		//目标代码段偏移(64:32)
	u32_t 	reserve;			//保留位
}__attribute__((packed))IntGate;	//16B

/* IDT：中断门描述符表 */
extern IntGate idt[INTMAX];


/* 中断描述符表头 */
typedef struct IDTR_T{
	u16_t len;	//长度
	u64_t base;	//基址
}__attribute__((packed))idtr_t;		//10B

/* IDTR */
extern idtr_t idtr;


#endif //__IDT_T_H