/**
 * 系统调用模块
 * 类型定义
 */
#ifndef __SYSCALL_T_H
#define __SYSCALL_T_H


/**
 * 系统调用状态切换的核心是软中断指令int n
 */



/* 传递1个参数所用的宏 */
#define API_ENTRY_PARE1(intnr,rets,pval1) \
__asm__ __volatile__(\
	"movq %[inr],%%rax\n\t"\
	"movq %[prv1],%%rbx\n\t"\
	"int $255 \n\t"\
	"movq %%rax,%[retval] \n\t"\
	:[retval] "=r" (rets)\
	:[inr] "r" (intnr),[prv1]"r" (pval1)\
	:"rax","rbx","cc","memory"\
)

/* 传递2个参数所用的宏 */
#define API_ENTRY_PARE2(intnr,rets,pval1,pval2) \
__asm__ __volatile__(\
	"movq %[inr],%%rax \n\t"\
	"movq %[prv1],%%rbx \n\t"\
	"movq %[prv2],%%rcx \n\t"\
	"int $255 \n\t"\
	"movq %%rax,%[retval] \n\t"\
	:[retval] "=r" (rets)\
	:[inr] "r" (intnr),[prv1]"r" (pval1),\
	[prv2] "r" (pval2)\
	:"rax","rbx","rcx","cc","memory"\
)
    
/* 传递3个参数所用的宏 */   
#define API_ENTRY_PARE3(intnr,rets,pval1,pval2,pval3) \
__asm__ __volatile__(\
	"movq %[inr],%%rax \n\t"\
	"movq %[prv1],%%rbx \n\t"\
	"movq %[prv2],%%rcx \n\t"\
	"movq %[prv3],%%rdx \n\t"\
	"int $255 \n\t"\
	"movq  %%rax,%[retval] \n\t"\
	:[retval] "=r" (rets)\
	:[inr] "r" (intnr),[prv1]"g" (pval1),\
	[prv2] "g" (pval2),[prv3]"g" (pval3)\
	:"rax","rbx","rbx","rcx","rdx","cc","memory"\
)

/* 传递4个参数所用的宏 */
#define API_ENTRY_PARE4(intnr,rets,pval1,pval2,pval3,pval4) \
__asm__ __volatile__(\
	"movq %[inr],%%rax \n\t"\
	"movq %[prv1],%%rbx \n\t"\
	"movq %[prv2],%%rcx \n\t"\
	"movq %[prv3],%%rdx \n\t"\
	"movq %[prv4],%%rsi \n\t"\
	"int $255 \n\t"\
	"movq %%rax,%[retval] \n\t"\
	:[retval] "=r" (rets)\
	:[inr] "r" (intnr),[prv1]"g" (pval1),\
	[prv2] "g" (pval2),[prv3]"g" (pval3),\
	[prv4] "g" (pval4)\
	:"rax","rbx","rcx","rdx","rsi","cc","memory"\
)


#endif //__SYSCALL_T_H