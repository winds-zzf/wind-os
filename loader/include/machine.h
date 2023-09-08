/**
 * 机器信息
 * API定义
 */
#ifndef __MACHINE_H
#define __MACHINE_H


/**
 * 初始化机器信息结构体
 * mach:机器信息结构体
 */
void init_machine(Machine *mach);

/**
 * 显示机器信息结构体
 * mach:机器信息结构体
 */
void display_machine(Machine mach);

/**
 * 不能在头文件中直接定义函数，但是可以定义内置函数
 * callint: 
 * val1: 
 * val2: 
 */
INLINE void real_entry(u16_t callint,u16_t val1,u16_t val2){
    __asm__ __volatile__(
        "mov %0, %%ax\n"    //将callint参数值传递给AX寄存器
        "call realadr_call_entry\n"  // 调用汇编函数
        :
        : "r" (callint)
        : "ax"
    );
}


#endif //__MACHINE_H