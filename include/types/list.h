/**
 * 双向循环链表结构
 * API定义
 */
#ifndef __LIST_H
#define __LIST_H

/**
 * 可以考虑环状分配与合并
 * 比如说从双向循环链表中取出一个子链构成子双向循环链表，或者将一个双向循环链表合并到另一个双向循环链表中
 */


#include "list_t.h"

/**
 * 初始化双向环状链表 
 * list指向头结点，由于是双相链表，因此初始时prev和next都是指向头节点本身
 * 注意：这种方式的双向链是环状的，环状链的好处是，节点是平等的，谁都可以是头或位，删除也不用担心空指针异常
 */
INLINE void list_t_init(list_t *list){
	list->prev = list;
	list->next = list;
	return;
}


/**
 * 移除两个节点中间的子链
 * 只需给出子链的前驱节点和后继结点prev->(sub list)->next
 */
INLINE void __list_del(list_t *prev, list_t *next){
	next->prev = prev;
	prev->next = next;
	return;
}


/**
 * 在指定的两个节点之间插入新节点
 * 由于是双向链表，与prev和next分别建立两个连接，共四个赋值语句
 */
INLINE void __list_add(list_t *new, list_t *prev, list_t *next){
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
	return;
}

/**
 * 前插法插入新节点（默认） 
 * prev = head
 * next = head->next
 */
INLINE void list_add(list_t *new, list_t *head){
	__list_add(new, head, head->next);
	return;
}


/**
 * 尾插法插入新节点
 * prev = head->prev
 * next = head
 */
INLINE void list_add_tail(list_t *new, list_t *head){
	__list_add(new, head->prev, head);
	return;
}


/**
 * 将entry节点从所在链表中移除
 */
INLINE void __list_del_entry(list_t *entry){
	__list_del(entry->prev, entry->next);
	return;
}


/**
 * 将entry从所在链表中移除，并将entry节点初始化为一个链表
 */
INLINE void list_del(list_t *entry){
	__list_del(entry->prev, entry->next);
	list_t_init(entry);
	return;
}


/**
 * 将list节点移动到head节点后
 * 节点移动可以是跨链表，也可以是在同一个链表中
 */
INLINE void list_move(list_t *list, list_t *head){
	list_del(list);
	list_add(list, head);
	return;
}


/**
 * 将list节点移动到head节点前
 * 节点移动可以跨链表，也可以是在同一链表中
 */
INLINE void list_move_tail(list_t *list, list_t *head){
	list_del(list);					//从list中移出list节点
	list_add_tail(list, head);		//将list插入到head前面
	return;
}


/**
 * 判断链表是否为空
 */
INLINE bool_t list_is_empty(const list_t *head){
	if (head->next == head){
		return TRUE;
	}
	return FALSE;
}


/**
 * 仔细判断链表是否为空
 */
INLINE bool_t list_is_empty_careful(const list_t *head){
	list_t *next = head->next;
	if (next == head && next == head->prev){	//头结点指向自己
		return TRUE;
	}
	return FALSE;
}


/**
 * 节点是否是链表的第一个元素
 */
INLINE bool_t list_is_first(const list_t* list,const list_t* head){
	if(list == head->next){
		return TRUE;
	}
	return FALSE;
}


/**
 * 节点是否是最后一个元素
 */
INLINE bool_t list_is_last(const list_t* list,const list_t* head){
	if(list == head->prev){
		return TRUE;
	}
	return FALSE;
}


/**
 * 遍历这个链表
 * 从head的下一个开始，一直向后(next)走，直到到达head。cur只是循环变量 
 * head指向的节点不会被处理
 */
#define list_for_each(pos, head) \
	for(pos = (head)->next; pos != (head); pos = pos->next)


/**
 * 这个宏用于依次取出链表中的首元结点并执行删除，（如果在遍历时不执行删除的话，代码会陷入死循环）
 */
#define list_for_each_head_dell(pos, head) \
	for(pos = (head)->next; pos != (head); pos = (head)->next)


/**
 * 根据结构体成员地址获取结构体起始地址
 * ptr: 结构体成员指针
 * type:结构体类型名
 * member:结构体成员名
 */
#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))


/**
 *
 */
#define list_first_oneobj(head,o_type,o_member) \
	list_entry((head)->next,o_type,o_member)

/**
 * 获取当前对象的下一个相邻对象
 * pos是数据对象指针，而不是List指针，因此要保证链表上的数据对象类型相同
 */
#define list_next_entry(pos,type,member) \
	list_entry((pos)->member.next,type,member)

/**
 * 获取当前对象的前一个相邻对象
 * pos是数据对象指针，而不是List指针，因此要保证链表上的数据对象类型相同
 */
#define list_prev_entry(pos,type,member) \
	list_entry((pos)->member.prev,type,member)


#endif //__LIST_H