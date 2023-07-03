/**
 * 平台初始化
 * 类型定义
 */

#ifndef __PLATFORM_T_H
#define __PLATFORM_T_H

#define KERNEL_VIRADR_START 0xffff800000000000ULL	//内核虚拟地址起点

/* 机器信息结构体 */
typedef struct MACHINE{
	u64_t stack_addr;		//内核栈地址
	u64_t stack_size;		//内核栈大小
	u64_t page_addr;		//页表首地址
	u64_t page_size;		//页表总大小
	u64_t cpu_mode;		//CPU工作模式
	u64_t mem_size;		//内存大小
	u64_t e820_addr;		//e820数组地址
	u64_t e820_num;		//e820数组元素个数
	u64_t e820_size;		//e820数组大小
	u64_t font_addr;		//内核字体地址
	u64_t font_size;		//内核字体大小
	u64_t kernel_addr;		//内核文件大小
	u64_t kernel_size;		//内核文件大小
	u64_t viraddr_size;		//内核虚拟地址空间大小
	u64_t bitmap_addr;		//位示图位置(用于进行简单内存管理)
	u64_t bitmap_size;		//位示图大小
	u64_t checksum;		//校验和
}__attribute__((packed))Machine;


# endif //__PLATFORM_T_H
