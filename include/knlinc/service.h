/**
 * 系统服务模块
 */
#ifndef __SERVICE_H
#define __SERVICE_H


/**
 * 系统服务分发器函数
 * 由中断处理的框架函数调用
 */
sysstus_t krlservice(uint_t inr,void* sframe);


#endif //__SERVICE_H