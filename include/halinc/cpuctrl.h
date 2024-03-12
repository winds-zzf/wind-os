/**
 * CPU控制模块
 * API定义
 */
#ifndef __CPUCTRL_H
#define __CPUCTRL_H

#include "_stdio.h"

INLINE uint_t get_cpu_count() {
    uint_t cpu_count;

    __asm__ __volatile__(
        "mov $1, %%eax\n"
        "cpuid\n"
        "mov %%ebx, %0\n"
        : "=r" (cpu_count)
        :
        : "%eax", "%ebx", "%ecx", "%edx"
    );

    return (cpu_count >> 16) & 0xFF;
}

/**
 * 获取当前cpuid
 */
INLINE uint_t hal_retn_cpuid() {
	uint_t cpuid;
	__asm__ __volatile__("mov $1, %%eax\n\t"
		"cpuid\n\t"
		"mov %%ebx, %0\n\t"
		: "=r" (cpuid)
		:
		: "%eax", "%ebx", "%ecx", "%edx"
	);
	
	return 0;
}

#endif //__CPUCTRL_H