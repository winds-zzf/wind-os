;实现调用BIOS中断功能,16位实模式下的asm代码

%include "loader.inc"	;包含一个名为“ldrasmd.inc”的外部文件
global _start		;声明_start为全局标号，以便其他模块引用

[section .text]
[bits 16]		;设置代码位数为16位
_start:
_16_mode:
	mov	bp,0x20 		;0x20是指向GDT中的16位数据段描述符k16da_dsc
	mov	ds, bp
	mov	es, bp
	mov	ss, bp
	mov	ebp, cr0
	and	ebp, 0xfffffffe
	mov	cr0, ebp 		;CR0.P=0关闭保护模式，使用0xfffffffe将cr0寄存器最低位置0，其余位置1
	jmp	0:real_entry 	;刷新CS影子寄存器，真正进入实模式

real_entry:		;实模式入口标签
	mov bp, cs
	mov ds, bp
	mov es, bp
	mov ss, bp
	mov sp, 08000h	;实模式栈指针
	mov bp,func_table
	add ax,ax		;ax*2
	add bp,ax		;调用函数表中的汇编函数，ax是C函数传递过来的，用于指定使用哪个函数
	call [bp] 
	
	cli
	call disable_nmi
	;关不可屏蔽中断
	mov	ebp, cr0
	or	ebp, 1
	mov	cr0, ebp 		;CR0.P=1开启保护模式
	jmp dword 0x8 :_32bits_mode	;返回32位模式
[BITS 32]
_32bits_mode:
	mov bp, 0x10 		;kcode_dsc
	mov ds, bp
	mov ss, bp
	mov esi,[PM32_EIP_OFF]	;恢复中断前的EIP
	mov esp,[PM32_ESP_OFF]	;恢复中断前的ESP
	jmp esi			;返回到调用处（save_eip_jmp）继续执行pop gs，以恢复更多的寄存器，继而恢复到调用前的状态

[BITS 16]
DispStr:			;用于显示字符串的函数
	mov bp,ax
	mov cx,23
	mov ax,01301h
	mov bx,000ch
	mov dh,10
	mov dl,25
	mov bl,15
	int 10h
	ret
cleardisp:			;清屏函数
	mov ax,0600h     	;这段代码是为了清屏
	mov bx,0700h
	mov cx,0
	mov dx,0184fh
	int 10h			;调用的BOIS的10号
	ret

_getmmap:				;获取内存布局视图e820数组的函数
	push ds
	push es
	push ss
	mov esi,0
	mov dword[E820_SIZE],esi
	mov dword[E820_PADR],E820_ADR

	xor ebx,ebx
	mov edi,E820_ADR
loop:
	mov eax,0e820h	
	mov ecx,20 	
	mov edx,0534d4150h 
	int 15h 	
	jc .1

	add edi,20
	cmp edi,E820_ADR+0x1000
	jg .1
	inc esi
	cmp ebx,0
	jne loop			;循环获取e820map结构
	jmp .2
.1:
	mov esi,0
.2:
	mov dword[E820_SIZE],esi
	pop ss
	pop es
	pop ds
	ret

_read:			;读取硬盘的函数
	push ds
	push es
	push ss
	xor eax,eax
	mov ah,0x42
	mov dl,0x80
	mov si,RWHDPACK_ADR	;将货物获取的磁盘信息放入RWHDPACK_ADR下
	int 0x13
	jc  .err
	pop ss
	;else
	pop es
	pop ds
	ret
.err:
	mov ax,int131errmsg	;将字符串地址存入ax寄存器，然后调用DispStr子程序显示字符串
	call DispStr
	jmp $
	pop ss
	pop es
	pop ds
	ret

_get_input:
    mov ah, 0       ; 功能号0：等待键盘输入
    int 0x16        ; 调用BIOS中断0x16，等待键盘输入
    
    ; 检查AL寄存器，判断是否有可用的键盘输入
    cmp al, 0
    je .no_input    ; 如果AL为0，则没有可用的键盘输入，跳转到.no_input标签
    
    ; 回显用户输入的字符
    mov ah, 0x0E    ; 功能号0x0E：在TTY模式下显示字符
    mov bh, 0       ; 页号，通常为0
    mov bl, 7       ; 前景色为白色，背景色为黑色
    int 0x10        ; 调用BIOS中断0x10，显示字符
    
    ; 检查回车键是否被按下，若是则结束输入
    cmp al, 0x0D
    je .input_done
    
    ; 继续读取下一个字符
    jmp _get_input

.no_input:
    ; 在此可以添加等待用户输入的代码，例如：暂停程序执行一段时间或显示提示信息
    
    ; 继续读取下一个字符
    jmp _get_input

.input_done:
    ret             ; 返回到调用该函数的地方

disable_nmi:		;禁用nmi(不可屏蔽中断)的函数，不可屏蔽中断通常用于错误处理
	push ax
	in al, 0x70     ; port 0x70NMI_EN_PORT
	or al, 0x80	; disable all NMI source
	out 0x70,al	; NMI_EN_PORT
	pop ax
	ret

func_table:		;函数表，包含要调用的汇编函数的地址(标号)
	dw _getmmap		;获取内存布局视图的函数
	dw _read		;读取硬盘的函数
	dw _get_input		;

;下面是错误消息字符串
int131errmsg: db     "int 13 read hdsk error"
        db 0
