/**
 * IDT初始化模块
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

/**
 * 设置idtr结构体的值
 */
static void set_idtr(){
    idtr.base = (u64_t)idt;
    idtr.len = sizeof(idt) - 1;	//下标从0开始
    return;
}

/**
 * 将idtr结构体数据加载到IDTR寄存器中
 * idtr: Idtr结构体
 * 
 * 
 */
static void load_idtr(){
    __asm__ __volatile__(
        "lidt (%0) \n\t"		//lidt即load idt，用于加载idt到idtr寄存器中的指令
        :
        : "r"(&idtr)
        : "memory");
    return;
}

/**
 * 设置中断门描述符的值
 * vector: 向量也是中断号。u8_t对应正好256个中断
 * type: 中断门类型：中断门，陷阱门，调用门
 * handle: 中断处理程序的入口地址
 * privilege: 中断门的权限级别
 * 
 * 中断门描述符存储在内存中，由IDTR寄存器指向。
 * handle只是中断处理入口程序地址，它并没有在set_intGate中回调执行，因此并不是回调函数。
 * 这里只是把函数地址保存在了中断门中，在发生中断时被执行。
 */
static void set_intGate(u8_t vector, u8_t type, IntDispose idispose, u8_t privilege){
    IntGate *igate = &idt[vector];			//获取vector对应的中断门描述符
    u64_t addr = (u64_t)idispose;			//获取回调函数地址值
    //初始化中断门各字段
    igate->offset_low = addr & 0xffff;		//地址[15:0]
    igate->selector = SELECTOR_KERNEL_CS;	//中断程序是在代码段
    igate->dcount = 0;					//(保留[7:4],IST[3:0])清空即可
    igate->attr = (u8_t)(type | (privilege << 5));	//将各字段置位，其他字段包含在type中
    igate->offset_mid = (u16_t)((addr >> 16) & 0xffff);		//地址[31:16]
    igate->offset_high = (u32_t)((addr >> 32) & 0xffffffff);	//地址[63:32]
    igate->reserve = 0;		//保留字段清空即可
    return;
}

/**
 * 初始化中断门描述符表
 */
void idt_init(){
	/**
	 * 初始化：一开始把所有的中断处理入口程序设置为保留的通用处理程序,避免未知的中断异常类型发生后CPU无处可去
	 */
	for(u16_t i=0;i<INTMAX;i++){	//中断号&中断门类型&中断处理入口程序&中断类型
		set_intGate((u8_t)i,DA_386IGate,hxi_exc_general_intpfault,PRIVILEGE_KRNL);
	}
	/**
	 * 对已有的中断门描述符进一步设置，覆盖之前的通用中断处理程序
	 */
	set_intGate(INT_VECTOR_DIVIDE, DA_386IGate, exc_divide_error, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_DEBUG, DA_386IGate, exc_debug_exception, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_NMI, DA_386IGate, exc_nmi, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_BREAKPOINT, DA_386IGate, exc_breakpoint_exception, PRIVILEGE_USER);
	set_intGate(INT_VECTOR_OVERFLOW, DA_386IGate, exc_overflow, PRIVILEGE_USER);
	set_intGate(INT_VECTOR_BOUNDS, DA_386IGate, exc_bounds_check, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_INVAL_OP, DA_386IGate, exc_inval_opcode, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_COPROC_NOT, DA_386IGate, exc_copr_not_available, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_DOUBLE_FAULT, DA_386IGate, exc_double_fault, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_COPROC_SEG, DA_386IGate, exc_copr_seg_overrun, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_INVAL_TSS, DA_386IGate, exc_inval_tss, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_SEG_NOT, DA_386IGate, exc_segment_not_exist, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_STACK_FAULT, DA_386IGate, exc_stack_exception, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_PROTECTION, DA_386IGate, exc_general_protection, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_PAGE_FAULT, DA_386IGate, exc_page_fault, PRIVILEGE_KRNL);
	set_intGate(15,DA_386IGate,hxi_exc_general_intpfault,PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_COPROC_ERR, DA_386IGate, exc_copr_error, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_ALIGN_CHEK, DA_386IGate, exc_alignment_check, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_MACHI_CHEK, DA_386IGate, exc_machine_check, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_SIMD_FAULT, DA_386IGate, exc_simd_fault, PRIVILEGE_KRNL);
	//...
	set_intGate(INT_VECTOR_IRQ + 0, DA_386IGate, hxi_hwint00, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 1, DA_386IGate, hxi_hwint01, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 2, DA_386IGate, hxi_hwint02, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 3, DA_386IGate, hxi_hwint03, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 4, DA_386IGate, hxi_hwint04, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 5, DA_386IGate, hxi_hwint05, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 6, DA_386IGate, hxi_hwint06, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 7, DA_386IGate, hxi_hwint07, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 8, DA_386IGate, hxi_hwint08, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 9, DA_386IGate, hxi_hwint09, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 10, DA_386IGate, hxi_hwint10, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 11, DA_386IGate, hxi_hwint11, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 12, DA_386IGate, hxi_hwint12, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 13, DA_386IGate, hxi_hwint13, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 14, DA_386IGate, hxi_hwint14, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 15, DA_386IGate, hxi_hwint15, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 16, DA_386IGate, hxi_hwint16, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 17, DA_386IGate, hxi_hwint17, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 18, DA_386IGate, hxi_hwint18, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 19, DA_386IGate, hxi_hwint19, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 20, DA_386IGate, hxi_hwint20, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 21, DA_386IGate, hxi_hwint21, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 22, DA_386IGate, hxi_hwint22, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_IRQ + 23, DA_386IGate, hxi_hwint23, PRIVILEGE_KRNL);
	//...
	set_intGate(INT_VECTOR_APIC_IPI_SCHEDUL, DA_386IGate, hxi_apic_ipi_schedul, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_APIC_SVR, DA_386IGate, hxi_apic_svr, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_APIC_TIMER, DA_386IGate, hxi_apic_timer, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_APIC_THERMAL, DA_386IGate, hxi_apic_thermal, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_APIC_PERFORM, DA_386IGate, hxi_apic_performonitor, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_APIC_LINTO, DA_386IGate, hxi_apic_lint0, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_APIC_LINTI, DA_386IGate, hxi_apic_lint1, PRIVILEGE_KRNL);
	set_intGate(INT_VECTOR_APIC_ERROR, DA_386IGate, hxi_apic_error, PRIVILEGE_KRNL);
	//...
	set_intGate(INT_VECTOR_SYSCALL, DA_386IGate, exi_sys_call, PRIVILEGE_USER);

	//将中断门描述符表信息加载到IDTR
	set_idtr();
	load_idtr();
	
	return;
}