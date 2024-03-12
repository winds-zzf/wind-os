;GRUB识别头
FLAGS 	EQU 0x00010003	;多引导协议标志位
MAGIC1 	EQU 0x1BADB002	;grub1协议魔数
MAGIC2	EQU 0xe85250d6	;grub2协议魔数

global _start
extern grub_entry

[section .start.text]	;定义.start.text代码节
[bits 32]				;汇编程32位指令
_start:
	jmp _entry

;GRUB1 header
ALIGN 8
grub1_header:
	dd MAGIC1
	dd FLAGS
	dd -(MAGIC1+FLAGS)
	dd grub1_header
	dd _start
	dd 0
	dd 0
	dd _entry

;GRUB2 header
ALIGN 8
grub2_header:
	DD MAGIC2
	DD 0
	DD grub2_end-grub2_header
	DD -(MAGIC2+0+(grub2_end-grub2_header))
	DW 2,0
	DD 24
	DD grub2_header
	DD _start
	DD 0
	DD 0
	DW 3,0
	DD 12
	DD _entry
	DD 0
	DW 0,0
	DD 8
grub2_end:

;为了同时兼容grub1和grub2实现了两个grub header
;------------------------------------------------------------------------

ALIGN 8
_entry:
	;关中断
	cli
	;关不可屏蔽中断
	in al,0x70
	or al,0x80
	out 0x70,al
	;加载GDT
	lgdt [GDTR]		;加载全局段描述符表头到GDTR
	;长跳转
	jmp dword 0x8:_32bits_mode

_32bits_mode:
	mov ax,0x10 	;使用0x10初始化段寄存器，0x10是b32_code_descriptor的段选择子
	mov ds,ax
	mov ss,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	xor eax,eax
	xor ebx,ebx
	xor ecx,ecx
	xor edx,edx
	xor esi,esi
	xor edi,edi
	xor ebp,ebp
	xor esp,esp
	mov esp,0x7c00 	;初始化C语言运行需要的栈
	call grub_entry	;调用C语言函数
	jmp 0x200000		;从entry返回后，进入二级引导器模块loader.bin
	jmp $			;避免程序异常返回后无处可去

;全局段描述符表
GDT_START:
b32_null_descriptor: dq 0x0 				;占位，使index从1开始
b32_code_descriptor: dq 0x00cf9e000000ffff	;32位代码段描述符
b32_data_descriptir: dq 0x00cf92000000ffff	;32位数据段描述符
b16_code_descriptor: dq 0x00009e000000ffff	;32位形式的16位代码段描述符
b16_data_descriptor: dq 0x000092000000ffff	;32位形式的16位代码端描述符
GDT_END:

;GDTR值
GDTR:
GDTR_LEN dw GDT_END-GDT_START-1		;b16_null_descriptor不计算在长度之内
GDTR_BASE dd GDT_START



