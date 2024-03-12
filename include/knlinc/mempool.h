/**
 * 内存池模块
 * API定义
 */
#ifndef __MEMPOOL_H
#define __MEMPOOL_H


/**
 * 内存分配接口函数
 */
addr_t mempool_new(size_t size);

/**
 * 内存回收接口函数
 */
bool_t mempool_delete(addr_t addr,size_t size);

/**
 * 初始化内存池模块
 */
void mempool_init();

/**
 * 测试主函数
 */
void mempool_test_main();


#endif //__MEMPOOL_H