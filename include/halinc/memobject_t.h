/**
 * 内存对象模块
 * 类型定义
 */

#ifndef __MEMOBJECT_T_H
#define __MEMOBJECT_T_H


/**
 * 内存对象结构
 * 用于描述一个内存对象的信息，在内存对象还未分配处于空闲状态时，将MemObject暂存在内存对象中
 */
typedef struct MOBJECT{
	List 	hook;	//链表钩子
	uint_t	status;	//对象状态
	void* 	start;	//对象起始地址(为什么定义成void指针)
}MObject;


/**
 * 管理内存对象容器
 */
typedef struct MOBJCONTAINER{
	Spinlock 	lock;		//保护该结构的自旋锁
	List		hook;		//节点挂载链表钩子
	//状态\标志
	uint_t 	status;		//容器状态 
	uint_t	flag;		//容器标志
	//内存空间信息
	addr_t	start_addr;	//容器起始地址
	addr_t	end_addr;		//容器结束地址
	size_t	mobj_size;	//内存对象大小
	//链表信息
	List		assigns;		//容器中已分配对象链表
	uint_t	total_num;	//容器中总共对象个数
	List		frees;		//容器中空闲对象链表
	uint_t	frees_num;	//容器中空闲对象个数
	List		extends;		//容器中拓展结构的链表
	uint_t	extends_num;	//拓展链表数量
	/**
	 * 挂载分配给内存对象的内存页：如果你想要使用分配几个物理内存块，
	 * 必然要维护分配的内存页表，相当于是对应内存页的句柄，有了它，你才能使用这片内存
	 */
	struct{
		List basic;		//容器占用的基本内存页面
		List extends;		//容器占用的拓展内存页面
		size_t size;		//连续内存页数量
	}mems;
}MObjContainer;	//0xA8


/**
 * 管理内存对象容器拓展的内存
 */
typedef struct MOBJEXTAND{
	List 	hook;			//节点挂载链表钩子
	addr_t	start_addr;		//拓展区起始地址
	addr_t	end_addr;			//拓展区尾地址
	MObjContainer *container; 	//拓展对象所在的容器
}MObjExtend;	//0x28


/**
 * 内存对象容器挂载点，
 * 一个挂载点可以挂载不同类型的容器，但是容器内对象大小是一样的
 */
typedef struct MObjMount{
	List  	containers;		//挂载容器的链表
	uint_t	containers_num;	//容器数量
	MObjContainer *cache;		//最近一次使用容器（缓存机制，相当于每个挂载点有个影子寄存器）	
	size_t	size;			//容器内存对象的大小	
}MObjMount;


/**
 * 内存对象管理器
 */
#define MOBJMOUNT_MAX 64
typedef struct MOBJMANAGER{
	Spinlock		lock;				//保护该结构的自旋锁
	MObjMount		mounts[MOBJMOUNT_MAX];	//内存对象[32*1,32*64]
	uint_t 		number;				//容器数量
}MObjManager;


#endif //__MEMOBJECT_T_H