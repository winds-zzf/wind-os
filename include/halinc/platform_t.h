/**
 * 平台初始化
 * 类型定义
 */

#ifndef __PLATFORM_T_H
#define __PLATFORM_T_H


#define CPUCORE_MAX 1			//CPU核心数
#define MACHINE_ADR 0x100000		//机器信息结构体地址

/* 64位虚拟地址空间划分 */
#define USER_VIRADR_START	0x0000000000000000ULL	//用户虚拟地址空间起点&终点
#define USER_VIRADR_END		0x00007fffffffffffULL
#define RESV_VIRADR_START	0x0000800000000000ULL	//保留虚拟地址空间起点&终点
#define RESV_VIRADR_END		0xffff7fffffffffffULL
#define KRNL_VIRADR_START 	0xffff800000000000ULL	//内核虚拟地址空间起点&终点
#define KRNL_VIRADR_END 		0xffffffffffffffffULL

/* 16GB的线性映射空间 */
#define KRNL_MAP_VIRADR_START		KRNL_VIRADR_START	//被映射的虚拟地址空间地点&大小&终点		
#define KRNL_MAP_VIRADRESS_SIZE 	0x400000000
#define KRNL_MAP_VIRADR_END	 	(KRNL_MAP_VIRADR_START+KRNL_MAP_VIRADR_SIZE - 1)
#define KRNL_MAP_PHYADR_START	 	0x000000000		//被映射的物理地址空间起点&大小&终点
#define KRNL_MAP_PHYADRESS_SIZE 	0x400000000
#define KRNL_MAP_PHYADR_END	 	(KRNL_MAP_PHYADR_START+KRNL_MAP_PHYADR_SIZE - 1)


/* 虚拟地址<->物理地址 */
#define MAP_RV(v) (KRNL_VIRADR_START+(v))			//虚拟地址->物理地址
#define MAP_VR(r) ((r)-KRNL_VIRADR_START)			//物理地址->虚拟地址

/**
 * 机器信息结构体
 * note:
 *  Machine主要用来存放动态分配内存的数据信息
 *  结构体属于都使用最基本的u64_t类型，以保证最大的兼容性
 * （loader层结构体和此处的结构体必须保持一致，因此可维护性稍差，可以使用适配器模式进行适配）
 * （Machine中既有虚拟地址，也有物理地址）
 * （注意，栈是向低地址处增长的）
 */
typedef struct MACHINE{
	u64_t stack_addr;		//内核栈地址
	u64_t stack_size;		//内核栈大小

	//must physical address
	u64_t mmu_addr;		//mmu页表首地址
	u64_t mmu_size;		//mmu页表总大小
	
	u64_t cpu_mode;		//CPU工作模式
	u64_t mem_size;		//内存大小

	//must virtual address
	u64_t e820s_addr;		//内存视图地址
	u64_t e820s_num;		//内存视图元素数
	u64_t e820s_size;		//内存视图大小

	u64_t image_addr;		//内核映像文件地址
	u64_t image_size;		//内核映像文件大小
	
	u64_t font_addr;		//内核字体地址
	u64_t font_size;		//内核字体大小
	
	u64_t kernel_addr;		//内核文件大小
	u64_t kernel_size;		//内核文件大小
	
	u64_t viraddr_size;		//内核虚拟地址空间大小

	//must virtual address
	u64_t pages_addr;		//内存页表首地址
	u64_t pages_num;		//内存页表数量
	u64_t pages_size;		//内存页表大小

	//must virtual address
	u64_t areas_addr;		//内存区地址
	u64_t areas_num;		//内存区数量
	u64_t areas_size;		//内存区大小

	//must physical address
	u64_t next_addr;		//指向下一个4K对齐的内存地址
	
	u64_t checksum;		//校验和
}__attribute__((packed))Machine;
extern Machine machine;	//头文件中声明为全局变量


# endif //__PLATFORM_T_H
