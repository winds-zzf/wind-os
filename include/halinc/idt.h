/**
 * IDT初始化模块
 * API定义
 */

#ifndef __IDT_H
#define __IDT_H


/**
 * 初始化中断描述符表IDT
 */
void idt_init();

/**
 * 中断处理函数声明
 * 具体函数体由汇编实现
 * hwint:Hardware interrupt:硬件中断
 */
void exc_divide_error();			//处理除法错误
void exc_debug_exception();		//处理调试异常
void exc_nmi();				//处理非屏蔽中断(nmi:nonmaskable interrupt)
void exc_breakpoint_exception();	//处理断点异常
void exc_overflow();			//处理溢出异常
void exc_bounds_check();			//处理边界检查异常
void exc_inval_opcode();			//处理无效操作码异常
void exc_copr_not_available();	//处理协处理器不可用异常
void exc_double_fault();			//处理双重中断错误
void exc_copr_seg_overrun();		//处理协处理器段超限异常
void exc_inval_tss();			//处理无效TSS异常
void exc_segment_not_exist();		//处理段不存在异常
void exc_stack_exception();		//处理堆栈异常
void exc_general_protection();	//处理通用保护异常
void exc_page_fault();			//处理页错误异常
void exc_copr_error();			//处理协处理器错误异常
void exc_alignment_check();		//处理对齐检查异常
void exc_machine_check();		//处理机器检查异常
void exc_simd_fault();			//处理simd错误异常
void hxi_exc_general_intpfault();	//处理通用中断/异常
void hxi_hwint00();				//处理硬件中断0~23
void hxi_hwint01();
void hxi_hwint02();
void hxi_hwint03();
void hxi_hwint04();
void hxi_hwint05();
void hxi_hwint06();
void hxi_hwint07();
void hxi_hwint08();
void hxi_hwint09();
void hxi_hwint10();
void hxi_hwint11();
void hxi_hwint12();
void hxi_hwint13();
void hxi_hwint14();
void hxi_hwint15();
void hxi_hwint16();
void hxi_hwint17();
void hxi_hwint18();
void hxi_hwint19();
void hxi_hwint20();
void hxi_hwint21();
void hxi_hwint22();
void hxi_hwint23();
void hxi_apic_ipi_schedul();		//处理APIC IPI调度中断
void hxi_apic_svr();			//处理APIC SVR中断
void hxi_apic_timer();			//处理APIC定时器中断
void hxi_apic_thermal();			//处理APIC热敏中断
void hxi_apic_performonitor();	//处理APIC性能监视中断
void hxi_apic_lint0();			//处理APIC LINE0中断
void hxi_apic_lint1();			//处理APIC LINE0中断
void hxi_apic_error();			//处理APIC错误中断
void exi_sys_call();			//处理系统调用
void asm_ret_from_user_mode();	//从用户模式返回


#endif //__IDT_H