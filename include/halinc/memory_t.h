/**
 * 内存初始化
 * 类型定义
 */

#ifndef __MEMORY_T_H
#define __MEMORY_T_H


#define RAM_USABLE		1	//可用内存
#define RAM_RESERVED 	2	//保留内存
#define RAM_ACPIDATA 	3	//ACPI data
#define RAM_ACPINVS		4	//ACPI NVS
#define RAM_UNUSABLE 	5	//坏内存
/* e820数组结构定义 */
typedef struct E820{
	u64_t addr;	//内存区域起始地址
	u64_t size;	//内存区域大小
	u32_t type;	//内存区域类型
}__attribute__((packed))E820;		//压缩存储结构体


/* 内存视图结构定义 */
typedef struct MEMORY{
	Spinlock lock;		//保护结构体的自旋锁
	u32_t type;		//内存地址空间类型
	u64_t addr;		//内存空间的开始地址
	u64_t size;		//内存空间的大小
	u64_t end;		//内存空间的结束地址
}Memory;


# endif //__MEMORY_T_H
