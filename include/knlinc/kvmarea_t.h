/**
 *
 */
#ifndef __KVMAREA_T_H
#define __KVMAREA_T_H


#include "kvmbox_t.h"

/*考虑将命名更改为zone*/
/* 虚拟内存地址区间 */
typedef struct KVMAREA{
	Spinlock 	lock;	//保护自身结构的锁
	List		hook;	//挂载钩子
	
	u64_t	flag;	//相关标志
	u64_t	limits;	//
	u32_t	mapType;	//映射类型

	//该虚拟内存区间的上下界
	addr_t	start;	//开始地址
	addr_t	end;		//结束地址(尾字节的下一个地址)
	
	void*	father;	//指向它的上层结构
	KvmBox*	box;		//管理这个结构对应的物理页面（使用指针延迟初始化）
	/**
	 * other fields
	 */
}KvmArea;

/**
 * 虚拟内存地址空间(准备更名为虚拟内存段)
 */
typedef struct KVMSPACE{
	Spinlock	lock;		//保护自身结构的锁
	uint_t	flag;		//相关标志
	
	List		areas;		//虚拟地址区间链表
	uint_t 	areasNum;		//虚拟地址区间计数

	void*	father;		//指向它的上层结构（暂未定义类型）

	//快捷访问（类似于快捷方式、Cache）
	KvmArea*	startArea;	//开始的虚拟地址空间
	KvmArea*	endArea;		//结束的虚拟地址空间
	KvmArea*	curArea;		//当前的虚拟地址空间

	//就是KvmSpace所表示的虚拟内存空间的边界，包括内存区间的划分都是在此范围内
	addr_t	freeStart;	//能分配的开始虚拟地址
	addr_t	freeEnd;		//能分配的结束虚拟地址(最后一个字节)
	
	void*	private;		//私有数据数据指针
	void*	extend;		//拓展数据指针
}KvmSpace;


#endif //__KVMAREA_T_H
