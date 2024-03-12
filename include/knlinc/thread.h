/**
 * 进程管理模块
 * API定义
 */
#ifndef __THREAD_H
#define __THREAD_H

#include "thread_t.h"


/**
 *
 */
thread_t* new_thread();

/**
 * 
 */
bool_t del_thread();

/**
 *
 */
void krnl_stack_init(thread_t* thread,void* entry, cpuflag_t cpuflags);

/**
 *
 */
thread_t* krlnew_thread(void* filerun,u64_t flag,u64_t privilege,u64_t priority,size_t usrstksz,size_t krlstksz);


#endif //__THREAD_H