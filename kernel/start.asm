;编译时对用户程序进行处理

extern main	;用户程序主函数main
extern exit	;
global _start	;进程运行入口点
[BITS 64]
_start:
	xor rax,rax	;清零rax
	mov rax,main	;将main函数地址存入rax
	call rax		;调用main函数并将返回值存入rax
	mov rsi,rax	;将返回值存入rsi
exit_loop:
	mov rax,exit	;将exit函数地址存入rax
	call rax		;调用exit函数
	jmp exit_loop	;循环调用exit，让程序一直运行下去，直到被外部强制终止