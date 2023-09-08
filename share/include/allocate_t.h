/**
 * 简单内存分配模块
 * 类型定义
 */

#ifndef __ALLOCATE_T_H
#define __ALLOCATE_T_H


//块信息
#define BLOCK_SIZE 	0x1000	//块大小4KB
#define ATTR	0x80000000	//取位常数

//宏函数
#define GET_SIZE(size) (((size)-1)/BLOCK_SIZE+1)		//求块数
#define GET_NUM(addr) ((addr)/BLOCK_SIZE)			//求块号
#define GET_ADR(size) ((size)*BLOCK_SIZE)			//求地址

/*内存管理结构体*/
typedef struct MEMMGR{
	u64_t maps_addr;		//位示图起始地址
	u64_t maps_size;		//位示图大小
	u64_t mem_addr;		//内存起始位置
	u64_t mem_size;		//内存大小
	u64_t block_size;		//内存块大小
	u64_t block_num;		//内存块数
}Memmgr;

#endif //__ALLOCATE_T_H

/**
 * 存在的问题
 * 1.位示图不够灵活，不能够拓展管理内存的
 * 2.没有将信息结构化
 * 3.命名没有统一化
 * 4.
 */