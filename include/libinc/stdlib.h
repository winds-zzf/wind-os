/**
 * 功能：常用实用函数
 * 描述：提供了大量实用的函数。
 */
#ifndef __STDLIB_T_H
#define __STDLIB_T_H


/**
 * malloc(),calloc(),realloc(),free()是C语言标准库中提供的函数，用于在C程序堆中分配和释放内存
 * 
 */





/**
 * 分配所需的内存空间，并返回一个指向它的指针。
 */
void* malloc(size_t size);

/**
 * 分配所需的内存空间，并返回一个指向它的指针。
 */
void* calloc(size_t* nitems,size_t size);

/**
 * 尝试重新调整之前调用 malloc 或 calloc 所分配的 ptr 所指向的内存块的大小。
 */
void* realloc(void *ptr,size_t size);

/**
 * 释放之前调用 calloc、malloc 或 realloc 所分配的内存空间。
 */
void free(void* ptr);


#endif //__STDLIB_T_H