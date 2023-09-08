/**
 * 简单内存分配模块
 * API定义
 */

#ifndef __ALLOCATE_H
#define __ALLOCATE_H


 /**
  * 分配内存4k对齐的内存地址
  * size:大小
  * 返回分配的地址
  */
addr_t allocate(const size_t size);

/**
 * 释放内存
 * addr:地址
 * size:大小
 */
bool_t release(const addr_t addr, const size_t size);

/**
 * 标记已经被占用的内存区域,相当于这部分是保留的内存
 * addr:地址
 * size:大小
 */
bool_t reserve(const addr_t addr, const size_t size);

/**
 * 初始化bitmap
 * maps_addr: 位示图数组地址
 * mem_addr: 被管理的内存起始地址
 * size: 被管理内存区域的大小
 */
void allocate_init(addr_t maps_addr,addr_t mem_addr,size_t size);

/**
 * 压缩显示bitmap
 */
void allocate_display();


#endif //__ALLOCATE_H