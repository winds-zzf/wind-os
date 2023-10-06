/**
 * 页面盒子，用来管理内空间对应的物理页面
 * 类型定义
 */
#ifndef __KVMBOX_T_H
#define __KVMBOX_T_H


#define KMBOX_CACHE_MAX (0x1000)	//4096	//最大缓存个数（这两个值为什么这样设置？）
#define KMBOX_CACHE_MIN	(0x60)	//64		//最小缓存个数
/* 页面盒子的头 */
typedef struct KVMBOXS{
	Spinlock	lock;		//保护自身结构的锁
	List		hook;		//链表钩子
	
	u64_t	status;		//自身状态
	u64_t	flag;		//相关标志
	
	List		boxs;		//挂载盒子的链表
	uint_t	boxsNum;		//链表上挂载盒子数量

	List		caches;		//缓存链表
	uint_t	cachesNum;	//当前缓存个数
	uint_t	cachesMax;	//最大缓存个数，超过了就要释放
	uint_t	cachesMin;	//最小缓存个数（为什么要有最小缓存个数？可能是在清缓存的时候设置的下限）

	void*	extend;		//拓展数据指针
}KvmBoxs;
/**
 * 缓存box是为了防止频繁分配、释放box结构带来的系统功能抖动
 * 同时，缓存几十个box结构下次可以取出即用不必再找内核申请(避免频繁切换)，
 * 这样可以大大提高性能 
 */

/* Count模块 */
typedef struct COUNTER{
	Spinlock lock;
}Counter;


/* 页面盒子 */
typedef struct KVMBOX{
	Spinlock 	lock;		//保护自身结构的锁
	List		hook;		//链表钩子
	
	uint_t	flag;		//相关标志
	uint_t	status;		//自身状态
	u64_t	type;		//类型
	
	Counter	shareCount;	//共享计数器（类型待定）
	
	List		pages;		//盒子对应的物理页面MemPage
	uint_t	pagesNum;		//盒子对应物理页面数

	KvmBoxs*	father;		//指向上层结构

	void*	fileNode;		//指向文件描述符节点
	void*	pager;		//指向分页器，暂时不使用
	
	void*	extend;		//自身拓展数据指针
}KvmBox;


#endif //__KVMBOX_T_H