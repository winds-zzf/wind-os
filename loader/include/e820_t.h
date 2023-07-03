/**
 * e820数组
 * 类型定义
 */
#ifndef __E820_T_H
#define __E820_T_H


#define RAM_USABLE		1		//可用内存
#define RAM_RESERVED	2		//保留内存
#define RAM_ACPIDATA	3		//ACPI data相关的 
#define RAM_ACPINVS		4		//ACPI NVS
#define RAM_UNUSABLE	5		//不可用内存

//e820数组结构定义，存储内存视图
typedef struct E820{
	u64_t addr;	//内存区域起始地址
	u64_t size;	//内存区域大小
	u32_t type;	//内存区域类型
}__attribute__((packed))E820;		//压缩存储结构体

#define BASE_ADR	0x2000U		//基地址
#define E820_SIZE 	(BASE_ADR+64)	//E820数组大小
#define E820_PADR	(BASE_ADR+68)	//E820数组指针地址
#define E820_ADR	(0x5000U)		//E820数组地址


#endif //__E820_T_H
