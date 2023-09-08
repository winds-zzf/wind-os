extern syscalls_table

;外部函数(中断分发器)
extern hal_hwint_allocator
extern hal_fault_allocator
extern hal_syscal_allocator

;全局标号，供外部调用中断处理程序
global	exc_divide_error
global	exc_debug_exception
global	exc_nmi
global	exc_breakpoint_exception
global	exc_overflow
global	exc_bounds_check
global	exc_inval_opcode
global	exc_copr_not_available
global	exc_double_fault
global	exc_copr_seg_overrun
global	exc_inval_tss
global	exc_segment_not_exist
global	exc_stack_exception
global	exc_general_protection
global	exc_page_fault
global	exc_copr_error
global	exc_alignment_check
global	exc_machine_check
global	exc_simd_fault
global	hxi_exc_general_intpfault
global	hxi_hwint00
global	hxi_hwint01
global	hxi_hwint02
global	hxi_hwint03
global	hxi_hwint04
global	hxi_hwint05
global	hxi_hwint06
global	hxi_hwint07
global	hxi_hwint08
global	hxi_hwint09
global	hxi_hwint10
global	hxi_hwint11
global	hxi_hwint12
global	hxi_hwint13
global	hxi_hwint14
global	hxi_hwint15
global	hxi_hwint16
global	hxi_hwint17
global	hxi_hwint18
global	hxi_hwint19
global	hxi_hwint20
global	hxi_hwint21
global	hxi_hwint22
global	hxi_hwint23
global	hxi_apic_ipi_schedul
global	hxi_apic_svr
global	hxi_apic_timer
global	hxi_apic_thermal
global	hxi_apic_performonitor
global	hxi_apic_lint0
global	hxi_apic_lint1
global 	hxi_apic_error
global	exi_sys_call
global	asm_ret_from_user_mode

%define	INT_VECTOR_IRQ				0x20
%define	INT_VECTOR_APIC_IPI_SCHEDUL 	0xe0
%define	INT_VECTOR_APIC_SVR 		0xef
%define	INT_VECTOR_APIC_TIMER		0xf0
%define	INT_VECTOR_APIC_THERMAL		0xf1
%define	INT_VECTOR_APIC_PERFORM		0xf2
%define	INT_VECTOR_APIC_LINTO		0xf3
%define	INT_VECTOR_APIC_LINTI		0xf4
%define	INT_VECTOR_APIC_ERROR		0xf5

%define	_KERNEL_DS 	0x10				;
%define	_NOERRO_CODE 	0xffffffffffffffff	;没有错误码

;保存中断后的寄存器(该宏指令限本模块使用)
%macro	SAVEALL	0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rsi
	push rdi
	
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	
	xor r14,r14
	
	mov r14w,ds
	push r14
	mov r14w,es
	push r14
	mov r14w,fs
	push r14
	mov r14w,gs
	push r14
	
	;push    ds
	;push    es
	;push    fs
	;push    gs
	
%endmacro

;恢复中断后的寄存器(该宏指令限本模块使用)
%macro	RESTOREALL	0
   	;pop	gs
	;pop	fs
	;pop	es
	;pop	ds
	pop r14
	mov gs,r14w
	pop r14 
	mov fs,r14w
	pop r14
	mov es,r14w
	pop r14
	mov ds,r14w
	
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	
	pop rdi
	pop rsi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
	
	iretq	;x86 架构中的一条指令，用于从中断处理程序返回到被中断的程序，并恢复被中断程序的状态。
%endmacro

;保存异常下的寄存器(该宏指令限本模块使用)
%macro	SAVEALLFAULT 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rsi
	push rdi
	
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	xor r14,r14
	mov r14w,ds
	push r14
	mov r14w,es
	push r14
	mov r14w,fs
	push r14
	mov r14w,gs
	push r14
%endmacro

;恢复异常下的寄存器(该宏指令限本模块使用)
%macro	RESTOREALLFAULT	0
	pop r14
	mov gs,r14w
	pop r14 
	mov fs,r14w
	pop r14
	mov es,r14w
	pop r14
	mov ds,r14w
	
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	
	pop rdi
	pop rsi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
	
	add rsp,8
	iretq
%endmacro

;没有错误码的CPU异常
%macro	SRFTFAULT 1
	push	  _NOERRO_CODE
	SAVEALLFAULT 			;保存异常下的寄存器
	mov r14w,0x10
	mov ds,r14w			;;重新设置各个段寄存器
	mov es,r14w
	mov fs,r14w
	mov gs,r14w

	mov 	rdi,%1 				;将第一个宏指令参数移入rdi
	mov 	rsi,rsp				;将栈指针传入rsi，栈中存储有中断时CPU自动保存的一些寄存器值
	call hal_fault_allocator		;执行异常分发器,将rdi,rsi的值分别作为函数实参
	RESTOREALLFAULT
%endmacro

;有错误码的CPU异常
%macro	SRFTFAULT_ECODE 1
	SAVEALLFAULT
	mov r14w,0x10
	mov ds,r14w
	mov es,r14w
	mov fs,r14w
	mov gs,r14w

	mov 	rdi,%1
	mov 	rsi,rsp
	call	hal_fault_allocator
	RESTOREALLFAULT
%endmacro

;硬件中断(HARDWare INTerrupt)
%macro	HARWINT	1
	SAVEALL
	
	mov r14w,0x10
	mov ds,r14w
	mov es,r14w
	mov fs,r14w
	mov gs,r14w

	mov	rdi, %1
	mov 	rsi,rsp
	call	hal_hwint_allocator		;执行中断分发器

	RESTOREALL
%endmacro

;系统调用宏指令
%macro  EXI_SCALL  0
	;push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rsi
	push rdi
	
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	
	xor r14,r14
	mov r14w,ds
	push r14
	mov r14w,es
	push r14
	mov r14w,fs
	push r14
	mov r14w,gs
	push r14
	
	mov r14w,0x10
	mov ds,r14w
	mov es,r14w
	mov fs,r14w
	mov gs,r14w

	call	hal_syscal_allocator	;执行系统调用分发器
	
	pop r14
	mov gs,r14w
	pop r14 
	mov fs,r14w
	pop r14
	mov es,r14w
	pop r14
	mov ds,r14w
	
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	
	pop rdi
	pop rsi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	;pop rax
	
	iretq
%endmacro
