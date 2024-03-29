;二级引导器核心入口汇编部分
%include "loader.inc"	;相当于汇编文件的头文件

global _start
global realadr_call_entry
extern loader_entry

[section .text]
[bits 32]
_start:
_entry:
	cli
	;因为代码模块的改变，所以需要加载IDTR和并重新加载GDTR
	lgdt [GDTR]		;加载GDT到GDTR寄存器
	lidt [IDTR]		;加载IDT到IDTR寄存器,IDT为中断向量表
	jmp dword 0x8 :_32bits_mode		;长跳转到_32bits_mode，并刷新影子寄存器

_32bits_mode:
	mov ax, 0x10	; 数据段选择子(目的)
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	xor eax,eax
	xor ebx,ebx
	xor ecx,ecx
	xor edx,edx
	xor edi,edi
	xor esi,esi
	xor ebp,ebp
	xor esp,esp
	mov esp,0x90000
	call loader_entry		;调用ldrkrl_entry函数，二级引导器的主函数
	xor ebx,ebx
	jmp 0x2000000 			;跳转到0x2000000的内存处,执行Cosmos.bin模块。
	jmp $


;C语言环境下调用BIOS中断（ldrkrl_entry中调用的），需要先切换回16位实模式，调用完BIOS中断之后，再切换回保护模式
realadr_call_entry:
	pushad			;保存通用寄存器
	push	ds
	push	es
	push	fs
	push	gs 		;保存四个段寄存器
	call save_eip_jmp
	pop	gs
	pop	fs
	pop	es
	pop	ds 			;按照逆序取出4个段寄存器
	popad 			;恢复通用寄存器
	ret
save_eip_jmp:
	pop esi 		;保存调用save_eip_jmp时保存的EIP到ESI寄存器中
	mov [PM32_EIP_OFF],esi 		;把EIP保存到特定的内存空间中
	mov [PM32_ESP_OFF],esp 		;把ESP保存到特定的内存空间中
	jmp dword far [cpmty_mode]	;长跳转这里把cpmty_mode处的第一个4字节装入EIP，把其后的2字节装入CS，跳转后就进入了16位模式了
cpmty_mode:
	dd 0x1000
	dw 0x18 			;装入CS寄存器，段选择子
	jmp $ 			;死循环

GDT_START:
b32_null_descriptor: dq 0
b32_code_descriptor: dq 0x00cf9a000000ffff ;a-e
b32_data_descriptor: dq 0x00cf92000000ffff
b16_code_descriptor: dq 0x00009a000000ffff ;a-e
b16_data_descriptor: dq 0x000092000000ffff
GDT_END:

GDTR:
GDT_LEN	dw GDT_END-GDT_START-1	;GDT界限
GDT_BASE	dd GDT_START

;中段门描述符表的基地址和长度
IDTR:
IDT_LEN	dw 0x3ff 	;BIOS中断表的长度
IDT_BASE	dd 0 		;BIOS中断表的基地址