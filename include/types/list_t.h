/**
 * 双向循环链表结构
 * 类型定义
 */
#ifndef __LIST_T_H
#define __LIST_T_H

/**
 * 双向循环链表节点
 */
typedef struct LIST{
	struct LIST *prev,*next;		//前驱后继指针
}List;


/**
 * 关于为什么链表结点没有数据？
 * 以往我们都是将结点分为指针域和数据域两部分，数据域放在结点中，
 * 但是其实结点也可以放在数据结构体中。然后通过节点地址来确定数据地址。
 */

#endif //__LIST_T_H