/**
 * 机器信息模块
 * 类型定义
 */
#ifndef __MACHINE_T_H
#define __MACHINE_T_H


/* 机器信息结构体 */
typedef struct MACHINE{
	u64_t stack_addr;		//内核栈地址
	u64_t stack_size;		//内核栈大小
	
	u64_t mmu_addr;		//mmu页表首地址
	u64_t mmu_size;		//mmu页表总大小
	
	u64_t cpu_mode;		//CPU工作模式
	u64_t mem_size;		//内存大小
	
	u64_t e820s_addr;		//e820数组地址
	u64_t e820s_num;		//e820数组元素数
	u64_t e820s_size;		//e820数组大小
	
	u64_t image_addr;		//内核映像文件地址
	u64_t image_size;		//内核映像文件大小
	
	u64_t font_addr;		//内核字体地址
	u64_t font_size;		//内核字体大小
	
	u64_t kernel_addr;		//内核文件地址
	u64_t kernel_size;		//内核文件大小
	
	u64_t viraddr_size;		//内核虚拟地址空间大小
	
	u64_t pages_addr;		//内存页表首地址
	u64_t pages_num;		//内存页表数量
	u64_t pages_size;		//内存页表大小
	
	u64_t areas_addr;		//内存区地址
	u64_t areas_num;		//内存区数量
	u64_t areas_size;		//内存区大小

	u64_t next_addr;		//下一个4K对齐页
		
	u64_t checksum;		//校验和
}__attribute__((packed))Machine;


#endif //__MACHINE_T_H