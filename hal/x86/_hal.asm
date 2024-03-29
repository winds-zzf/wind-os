; 内核初始化入口文件

%define IA32_EFER 0C0000080H
%define PAGE_ADR 0x1000000
%define MACHINE_ADR 0x100000  
%define STACK_OFF 0
%define KERNEL_VIRADR_START 0xffff800000000000

global _start
extern hal_entry

[section .start.text]
[BITS 32]
_start:
	cli
	mov ax,0x10
	mov ds,ax
	mov es,ax
	mov ss,ax
	mov fs,ax
	mov gs,ax
   	lgdt [GDTR]        

;开启 PAE(PAGE)
    	mov eax, cr4
    	bts eax, 5                 	; CR4.PAE = 1
    	mov cr4, eax
    	mov eax, PAGE_ADR
   	mov cr3, eax	

;开启 64bits long-mode
    	mov ecx, IA32_EFER
    	rdmsr
    	bts eax, 8                  	; IA32_EFER.LME =1
    	wrmsr

;开启 PE 和 paging
    	mov eax, cr0
    	bts eax, 0                  	; CR0.PE =1
    	bts eax, 31

;开启 CACHE       
     btr eax,29				; CR0.NW=0
     btr eax,30				; CR0.CD=0  CACHE
     mov cr0, eax                 ; IA32_EFER.LMA = 1

;长模式跳转
     jmp 08:entry64

[BITS 64]
;设置长模式下的C语言运行环境
entry64:
	mov ax,0x10
	mov ds,ax
	mov es,ax
	mov ss,ax
	mov fs,ax
	mov gs,ax
	xor rax,rax
	xor rbx,rbx
	xor rbp,rbp
	xor rcx,rcx
	xor rdx,rdx
	xor rdi,rdi
	xor rsi,rsi
	xor r8,r8
	xor r9,r9
	xor r10,r10
	xor r11,r11
	xor r12,r12
	xor r13,r13
	xor r14,r14
	xor r15,r15
     mov rbx,MACHINE_ADR
     mov rax,KERNEL_VIRADR_START
     mov rcx,[rbx+STACK_OFF]
     add rax,rcx		;将栈物理地址转换为虚拟地址
     xor rcx,rcx
	xor rbx,rbx
	mov rsp,rax
	push 0 			; 为了清空栈，避免有残留数据影响后续操作
	push 0x8 			; 为了将栈指针设置为正确的位置，因为在进入长模式后栈的位置会发生变化
     mov rax,hal_entry  ; 使用了汇编语言中的伪指令hal_start，实际是一个符号，代表内核主函数的地址
	push rax 			; 将该hal_start地址压入栈中，以便在进入内核主函数时使用
     dw 0xcb48			; iretq指令编码，使用iretq为了从实模式或保护模式切换到长模式，并进入内核主函数
     jmp $			; 死循环。避免执行其他指令，保证内核始终处于运行状态

		
[section .start.data]
[BITS 32]
GDT_START:
enull_x64_dsc:	 dq 0	
ekrnl_c64_dsc:  dq 0x0020980000000000           ; 64-bit 内核代码段
ekrnl_d64_dsc:  dq 0x0000920000000000           ; 64-bit 内核数据段
euser_c64_dsc:  dq 0x0020f80000000000           ; 64-bit 用户代码段
euser_d64_dsc:  dq 0x0000f20000000000           ; 64-bit 用户数据段
GDT_END:

GDTR:
GDT_LEN	dw GDT_END-GDT_START-1		; GDT长度
GDT_BASE	dq GDT_START				; GDT界限

