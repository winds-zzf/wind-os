/**
 * 
 */
#ifndef __KVMAREA_H
#define __KVMAREA_H

#include "kvmarea_t.h"

/**
 * 初始化虚拟内存空间
 */
void kvmArea_init(KvmArea *area);


/**
 * 初始化虚拟内存空间
 */
void kvmSpace_init(KvmSpace *space);


/**
 * 测试：初始化用户虚拟内存空间
 */
bool_t init_userspace_kvmspace(KvmSpace* space);


/**
 * 创建一个KvmArea对象，并初始化
 * return: 创建好的KvmArea对象指针，如果为NULL，说明创建失败
 */
KvmArea* new_kvmArea();


/**
 * 释放KvmArea的存储空间
 * area: 待释放的area指针
 */
bool_t del_kvmArea(KvmArea* area);

/**
 * 创建一个KvmSpace对象，并初始化
 * return: 创建好的KvmSpace指针，如果为NULL，说明创建失败
 */
KvmSpace* new_kvmSpace();


/**
 * 释放KvmSpace的存储空间
 * space: 待释放的KvmSpace对象
 */
bool_t del_kvmSpace(KvmSpace *space);


#endif //__KVMAREA_H