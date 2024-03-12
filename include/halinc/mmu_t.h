/**
 * MMU管理模块
 * 类型定义
 */
#ifndef __MMU_T_H
#define __MMU_T_H


//物理地址中各级页目录索引偏移与掩码
#define MMU_IDX1_SHIFT 	(39)
#define MMU_IDX1_MASK 	(0x1ffUL)
#define MMU_IDX2_SHIFT 	(30)
#define MMU_IDX2_MASK 	(0x1ffUL)
#define MMU_IDX3_SHIFT	(21)
#define MMU_IDX3_MASK	(0x1ffUL)
#define MMU_IDX4_SHIFT 	(12)
#define MMU_IDX4_MASK	(0x1ffUL)

//页目录子目录项数
#define MMU_PT1_MAX 512		//一级页表长
#define MMU_PT2_MAX 512		//二级页表长
#define MMU_PT3_MAX 512		//三级页表长
#define MMU_PT4_MAX 512		//四级页表长
#define MMU_PT_SIZE 0x1000	//页表大小
#define MMU_P_SIZE 0x1000	//页大小

//一级页目录属性位
#define MMU_PT1_P		(1<<0)
#define MMU_PT1_RW		(1<<1)
#define MMU_PT1_US		(1<<2)
#define MMU_PT1_PWT		(1<<3)
#define MMU_PT1_PCD		(1<<4)
#define MMU_PT1_A		(1<<5)	//automatically set

//二级页目录属性位
#define MMU_PT2_P		(1<<0)
#define MMU_PT2_RW		(1<<1)
#define MMU_PT2_US		(1<<2)
#define MMU_PT2_PWT		(1<<3)
#define MMU_PT2_PCD		(1<<4)
#define MMU_PT2_A		(1<<5)	//automatically set
#define MMU_PT2_PS		(1<<7)

//三级页目录属性位
#define MMU_PT3_P		(1<<0)
#define MMU_PT3_RW		(1<<1)
#define MMU_PT3_US		(1<<2)
#define MMU_PT3_PWT		(1<<3)
#define MMU_PT3_PCD		(1<<4)
#define MMU_PT3_A		(1<<5)	//automatically set
#define MMU_PT3_PS		(1<<7)

//四级页目录属性位
#define MMU_PT4_P		(1<<0)
#define MMU_PT4_RW		(1<<1)
#define MMU_PT4_US		(1<<2)
#define MMU_PT4_PWT		(1<<3)
#define MMU_PT4_PCD		(1<<4)
#define MMU_PT4_A		(1<<5)	//automatically set
#define MMU_PT4_PAT		(1<<7)
#define MMU_PT4_G		(1<<8)	//automatically set


/**
 * 使用数据结构进行硬件抽象
 * ig:ignore
 * rv:reserve
 * 这里的标志位，是页表地址特有的，并不是说每个虚拟地址都有
 */

//一级页目录
/*  */
typedef struct PT1FLAGS_T{
	u64_t	p:1;		//0
	u64_t	rw:1;	//1
	u64_t	us:1;	//2
	u64_t	pwt:1;	//3
	u64_t	pcd:1;	//4
	u64_t	a:1;		//5
	u64_t	ig1:1;	//6
	u64_t	rv1:1;	//7
	u64_t	ig2:4;	//8
	u64_t	adr:40;	//12
	u64_t	rv2:11;	//52
	u64_t	xd:1;	//63
}__attribute__((packed))pt1flags_t;
/*  */
typedef struct PT1ENTRY_T{
	union{	//匿名共用体:直接使用共用体功能
		pt1flags_t 	flags;	//属性位
		u64_t 		entry;	//页表项
	}__attribute__((packed));
}__attribute__((packed))pt1entry_t;
/*  */
typedef struct PT1_T{
	pt1entry_t entries[MMU_PT1_MAX];
}__attribute__((packed))pt1_t;


//二级页目录
/*  */
typedef struct PT2FLAGS_T{
	u64_t	p:1;		//0
	u64_t	rw:1;	//1
	u64_t	us:1;	//2
	u64_t	pwt:1;	//3
	u64_t	pcd:1;	//4
	u64_t	a:1;		//5
	u64_t	ig1:1;	//6
	u64_t	ps:1;	//7
	u64_t	ig2:4;	//8
	u64_t	adr:40;	//12
	u64_t	ig3:11;	//52
	u64_t	xd:1;	//63
}__attribute__((packed))pt2flags_t;
/*  */
typedef struct PT2ENTRY_T{
	union{
		pt2flags_t	flags;
		u64_t		entry;
	}__attribute__((packed));
}__attribute__((packed))pt2entry_t;
/*  */
typedef struct PT2_T{
	pt2entry_t entries[MMU_PT2_MAX];
}__attribute__((packed))pt2_t;


//三级页目录
/*  */
typedef struct PT3FLAGS_T{
	u64_t	p:1;		//0
	u64_t	rw:1;	//1
	u64_t	us:1;	//2
	u64_t	pwt:1;	//3
	u64_t	pcd:1;	//4
	u64_t	a:1;		//5
	u64_t	ig1:1;	//6
	u64_t	ps:1;	//7
	u64_t	ig2:4;	//8
	u64_t	adr:40;	//12
	u64_t	ig3:11;	//52
	u64_t	xd:1;	//63
}__attribute__((packed))pt3flags_t;
/*  */
typedef struct PT3ENTRY_T{
	union{
		pt3flags_t	flags;
		u64_t		entry;
	}__attribute__((packed));
}pt3entry_t;
/*  */
typedef struct PT3_t{	
	pt3entry_t entries[MMU_PT3_MAX];
}__attribute__((packed))pt3_t;

//四级页目录
/*  */
typedef struct PT4FLAGS_T{
	u64_t	p:1;		//0
	u64_t	rw:1;	//1
	u64_t	us:1;	//2
	u64_t	pwt:1;	//3
	u64_t	pcd:1;	//4
	u64_t	a:1;		//5
	u64_t	ig1:1;	//6
	u64_t	pat:1;	//7
	u64_t	g:1;		//8
	u64_t	ig2:3;	//9
	u64_t	adr:40;	//12
	u64_t	ig3:11;	//52
	u64_t	xd:1;	//63
}__attribute__((packed))pt4flags_t;
/*  */
typedef struct PT4ENTRY_T{
	union{
		pt4flags_t	flags;
		u64_t		entry;
	};
}__attribute__((packed))pt4entry_t;
/*  */
typedef struct PT4{
	pt4entry_t entries[MMU_PT4_MAX];
}__attribute__((packed))pt4_t;

//CR3寄存器
typedef struct CR3FLAGS_T{
	u64_t	ig1:4;	//0
	u64_t	pwt:1;	//4
	u64_t	pcd:1;	//5
	u64_t	ig2:6;	//11
	u64_t	adr:40;	//51
	u64_t	ig3:12;	//63
}__attribute__((packed))cr3flags_t;

typedef struct CR3_T{
	union{
		cr3flags_t	flags;
		u64_t 		entry;
	}__attribute__((packed));
}__attribute__((packed))cr3_t;


/**
 * MMU综合管理结构体
 */
typedef struct MMU_T{
	spinlock_t	lock;	//保护此结构的自旋锁
	u64_t	status;	//状态位
	u64_t	flag;	//标志位
	
	cr3_t	cr3;		//cr3寄存器
	pt1_t	*pt1;	//一级页表指针（为什么要有这个字段，为什么要用指针）

	//管理各级页表所占用物理页的页描述符链表
	list_t		pt1s;	//一级页表链表
	list_t		pt2s;	//二级页表链表
	list_t		pt3s;	//三级页表链表
	list_t		pt4s;	//四级页表链表
	//链表长
	uint_t	pt1sNum;	//一级页表链表长
	uint_t	pt2sNum;	//二级页表链表长
	uint_t	pt3sNum;	//三级页表链表长
	uint_t	pt4sNum;	//四级页表链表长
}mmu_t;


#endif //__MMU_T_H