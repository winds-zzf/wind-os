/**
 * 系统时间模块
 * 类型定义
 */
#ifndef __KRLTIME_H
#define __KRLTIME_H

#include "service_t.h"

/**
 * 更新系统时间
 */
void systime_update();

/**
 * 系统调用服务例程
 */
sysstus_t sve_time(uint_t inr,sframe_t* sframe);


#endif //__KRLTIME_H