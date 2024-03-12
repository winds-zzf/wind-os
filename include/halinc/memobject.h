/**
 * 内存对象模块
 * API定义
 */

#ifndef __MEMOBJECT_H
#define __MEMOBJECT_H


/**
 * 初始化内存对象结构体
 */
void memobject_init();


/**
 * 内存分配函数(类似于malloc())
 * size: 申请的内存空间大小
 * return: 返回void*便于使用者根据需求强转成所需类型指针，类似于malloc()
 */
void* mobj_new(size_t size);


/**
 * 内存释放函数（类似于free()）
 * addr: 待释放内存地址，使用void*可接受任意类型的指针而不必显式转换
 * size: 待释放内存大小
 */
bool_t mobj_delete(void *addr,size_t size);


/**
 *
 */
void memobject_test_main();


#endif //__MEMOBJECT_H