/**
 * MMU页表模块
 * 类型定义
 * 在hal阶段，为了简化编程这里使用了2MB分页模式，后续会重新设置为4K分页
 */
#ifndef __REMMU_T_H
#define __REMMU_T_H


//CR3寄存器属性位
#define REMMU_PT1_SHIFT	39	//提取一级页目录索引右移位数
#define REMMU_PT2_SHIFT	30	//提取二级页目录索引右移位数
#define REMMU_PT3_SHIFT	21	//提取三级页目录索引右移位数
#define REMMU_PCD	(1<<5)	//是否开启页级Cache
#define REMMU_PWT	(1<<4)	//写回法、写直达法

//一级页目录属性位
#define REMMU_PT1_P  	(1<<0)	//页面存在位
#define REMMU_PT1_RW	(1<<1)	//页面读写位
#define REMMU_PT1_US	(1<<2)  	//用户/超级管理者页
#define REMMU_PT1_PWT	(1<<3)  	//写回，写直达
#define REMMU_PT1_PCD	(1<<4)  	//是否开启页级Cache
#define REMMU_PT1_A		(1<<5)   	//访问位，由MMU硬件设置
//二级页目录属性位
#define REMMU_PT2_P		(1<<0)  	//页面存在位
#define REMMU_PT2_RW	(1<<1)  	//页面读写位
#define REMMU_PT2_US	(1<<2)  	//用户\超级管理者页
#define REMMU_PT2_PWT	(1<<3)  	//写回，写直达
#define REMMU_PT2_PCD	(1<<4)  	//是否开启页级Cache
#define REMMU_PT2_A		(1<<5)  	//访问位，由MMU硬件设置
#define REMMU_PT2_PS	(1<<7)  	//2MB分页下，PS必须为1
//三级页目录属性位
#define REMMU_PT3_P		(1<<0)  	//页面存在位
#define REMMU_PT3_RW	(1<<1)   	//页面读写位
#define REMMU_PT3_US	(1<<2)  	//用户/超级管理者页
#define REMMU_PT3_PWT	(1<<3)  	//写回，写直达
#define REMMU_PT3_PCD	(1<<4)  	//是否开启页级Cache
#define REMMU_PT3_A		(1<<5)  	//访问位，由MMU硬件设置
#define REMMU_PT3_D		(1<<6)  	//页面脏位，页面写入数据时由MMU设置
#define REMMU_PT3_PS	(1<<7)  	//2MB分页下，PS必须为1
#define REMMU_PT3_G		(1<<8)   	//全局标志位
#define REMMU_PT3_PAT  	(1<<12) 	//页面属性表
//页目录项子目录项数
#define REMMU_PT1_SIZE	1		//一级页目录的子目录项数
#define REMMU_PT2_SIZE	16		//二级页目录的子目录项数
#define REMMU_PT3_SIZE	512		//三级页目录的子目录项数
#define REMMU_PT_SIZE	0x1000	//页目录大小：4KB
#define REMMU_P_SIZE	0x200000	//分页大小：2MB
#define REMMU_SIZE (18*REMMU_PT_SIZE)	//MMU页表总大小
#define REMMU_MAP_SIZE 	0x400000000	//16GB的线性映射区(只是虚拟地址的一部分)


#endif //__REMMU_T_H

