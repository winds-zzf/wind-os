/**
 * CPU控制模块
 * API实现
 */
#include"global.h"
#include"cpu_t.h"

static bool_t check_cpuid();
static bool_t check_longmode();

void cpu_init(Machine *mach){
	//检查是否支持cpuid指令
	if(!check_cpuid()){
		printk("Your computer is not support cpuid instruction. System is die.");
		HALT();	//主动死机
	}
	//使用cpuid检查是否支持long mode
	if(!check_longmode()){
		printk("Your computer is not support long mode. System is die.");
		HALT();	//主动死机
	}
	
	//填写机器信息
	mach->cpu_mode = 0x40;	//cpu mode：64位

	return;
}

void cpu_display(Machine mach){
	printk("=================================cpu info==================================\n");
	printk("cpu mode:0x%lx\n",mach.cpu_mode);
	printk("=================================cpu end.==================================\n");

	return;
}

/**
 * 通过改写Eflags寄存器的21位，观察其位的变化判断是否支持cpuid指令，为检查CPU是否支持长模式做准备
 */
static bool_t check_cpuid(){
    int rets = FALSE;
    __asm__ __volatile__(
        "movl $0x80000000, %%eax \n\t"
        "cpuid \n\t"
        "cmpl $0x80000001, %%eax \n\t"
        "setnb %b0 \n\t"
        "jb 1f \n\t"
        "movl $0x80000001, %%eax \n\t"
        "cpuid \n\t"
        "bt $29, %%edx  \n\t"
        "setcb %b0 \n\t"
        "1: \n\t"
        : "=q"(rets)
        :
        : "eax", "edx", "cc" // 添加操作数约束和cc(clobber)
    );

    return rets;
}

/**
 * 检查CPU是否支持长模式，需要借助cpuid指令
 */
static bool_t check_longmode() {
    bool_t rets = FALSE;
    __asm__ __volatile__(
        "movl $0x80000000, %%eax \n\t"
        "cpuid \n\t"
        "cmpl $0x80000001, %%eax \n\t"
        "setnb %b0 \n\t"
        "jb 1f \n\t"
        "movl $0x80000001, %%eax \n\t"
        "cpuid \n\t"
        "bt $29, %%edx  \n\t"
        "setcb %b0 \n\t"
        "1: \n\t"
        : "=q"(rets)
        :
        : "eax", "edx", "cc" // 添加操作数约束和cc(clobber)
    );

    return rets;
}