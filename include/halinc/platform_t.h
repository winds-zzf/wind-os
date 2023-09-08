/**
 * 平台初始化
 * 类型定义
 */

#ifndef __PLATFORM_T_H
#define __PLATFORM_T_H


#define CPUCORE_MAX 1			//CPU核心数
#define MACHINE_ADR 0x100000		//机器信息结构体地址
#define KERNEL_VIRADR_START 0xffff800000000000ULL	//内核虚拟地址起点
#define MAP_RV(v) (KERNEL_VIRADR_START+(v))			//虚拟地址->物理地址
#define MAP_VR(r) ((r)-KERNEL_VIRADR_START)			//物理地址->虚拟地址

/**
 * 机器信息结构体
 * note:
 *  Machine主要用来存放动态分配内存的数据信息
 *  结构体属于都使用最基本的u64_t类型，以保证最大的兼容性
 * （loader层结构体和此处的结构体必须保持一致，因此可维护性稍差，可以使用适配器模式进行适配）
 * （Machine中到底应该存放物理地址还是虚拟地址？机器信息结构体应该使用物理地址）
 */
typedef struct MACHINE{
	u64_t stack_addr;		//内核栈地址
	u64_t stack_size;		//内核栈大小
	
	u64_t page_addr;		//mmu页表首地址
	u64_t page_size;		//mmu页表总大小
	
	u64_t cpu_mode;		//CPU工作模式
	u64_t mem_size;		//内存大小
	
	u64_t e820s_addr;		//e820数组地址
	u64_t e820s_num;		//e820数组元素个数
	u64_t e820s_size;		//e820数组大小
	
	u64_t font_addr;		//内核字体地址
	u64_t font_size;		//内核字体大小
	
	u64_t kernel_addr;		//内核文件大小
	u64_t kernel_size;		//内核文件大小
	
	u64_t viraddr_size;		//内核虚拟地址空间大小
	
	u64_t pages_addr;		//内存页表首地址
	u64_t pages_num;		//内存页表数量
	u64_t pages_size;		//内存页表大小
	
	u64_t bitmap_addr;		//位示图位置(用于进行简单内存管理)
	u64_t bitmap_size;		//位示图大小
	
	u64_t checksum;		//校验和
}__attribute__((packed))Machine;
extern Machine machine;	//头文件中声明为全局变量


# endif //__PLATFORM_T_H
