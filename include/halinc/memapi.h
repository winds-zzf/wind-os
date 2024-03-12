/**
 * 内存接口
 * API定义
 */

#ifndef __MEMAPI_H
#define __MEMAPI_H


/**
 * allocating continous pages
 */
MemPage* mem_divide(memmgr_t *mgr,size_t requestSize,size_t *realSize,uint_t areaType,uint_t flags);

/**
 * 懒分配；从应用区分配一个物理页，如果应用区没有内存，那就暂时从内核区申请分配
 */
MemPage* mem_divide_apps(memmgr_t *mgr);

/**
 * recycling continuous pages
 */
bool_t mem_merge(memmgr_t *mgr,MemPage *pages,size_t size);

/**
 * size向上2阶对齐，如果size=0,返回值也为0
 */
sint_t get_order(size_t size);

/**
 * 测试函数入口
 */
void memapi_test_main();


#endif //__MEMAPI_H