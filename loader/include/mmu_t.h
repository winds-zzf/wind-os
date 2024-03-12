/**
 * MMU页表模块
 * 类型定义
 *
 * 说明：
 * · 为了简化编程这里使用了2MB分页模式
 */
#ifndef __MMU_T_H
#define __MMU_T_H


//CR3寄存器标志字段
#define MMU_PT1_SHIFT	39	//提取一级页目录索引右移位数
#define MMU_PT2_SHIFT	30	//提取二级页目录索引右移位数
#define MMU_PT3_SHIFT	21	//提取三级页目录索引右移位数
#define MMU_PCD	(1<<5)	//是否开启页级Cache
#define MMU_PWT	(1<<4)	//写回法、写直达法

//一级页目录标志字段
#define MMU_PT1_P  	(1<<0)	//页面存在位
#define MMU_PT1_RW	(1<<1)	//页面读写位
#define MMU_PT1_US	(1<<2)  	//用户/超级管理者页
#define MMU_PT1_PWT	(1<<3)  	//写回，写直达
#define MMU_PT1_PCD	(1<<4)  	//是否开启页级Cache
#define MMU_PT1_A	(1<<5)   	//访问位，由MMU硬件设置

//二级页目录标志字段
#define MMU_PT2_P	(1<<0)  	//页面存在位
#define MMU_PT2_RW	(1<<1)  	//页面读写位
#define MMU_PT2_US	(1<<2)  	//用户\超级管理者页
#define MMU_PT2_PWT	(1<<3)  	//写回，写直达
#define MMU_PT2_PCD	(1<<4)  	//是否开启页级Cache
#define MMU_PT2_A	(1<<5)  	//访问位，由MMU硬件设置
#define MMU_PT2_PS	(1<<7)  	//2MB分页下，PS必须为1

//三级页目录标志字段
#define MMU_PT3_P		(1<<0)  	//页面存在位
#define MMU_PT3_RW		(1<<1)   	//页面读写位
#define MMU_PT3_US		(1<<2)  	//用户/超级管理者页
#define MMU_PT3_PWT		(1<<3)  	//写回，写直达
#define MMU_PT3_PCD		(1<<4)  	//是否开启页级Cache
#define MMU_PT3_A		(1<<5)  	//访问位，由MMU硬件设置
#define MMU_PT3_D		(1<<6)  	//页面脏位，页面写入数据时由MMU设置
#define MMU_PT3_PS		(1<<7)  	//2MB分页下，PS必须为1
#define MMU_PT3_G		(1<<8)   	//全局标志位
#define MMU_PT3_PAT  	(1<<12) 	//页面属性表

//页目录项子目录项数
#define MMU_PT1_SIZE	1		//一级页目录的子目录项数
#define MMU_PT2_SIZE	16		//二级页目录的子目录项数
#define MMU_PT3_SIZE	512		//三级页目录的子目录项数
#define MMU_PT_SIZE		0x1000	//页目录大小：4KB
#define MMU_P_SIZE		0x200000	//分页大小：2MB
#define MMU_SIZE (18*MMU_PT_SIZE)	//MMU页表总大小


#endif //__MMU_T_H
