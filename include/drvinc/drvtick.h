/**
 * 8254时钟
 * API定义
 */
#ifndef __DRVTICK_H
#define __DRVTICK_H


/**
 * 驱动程序入口函数
 * 入口函数执行驱动程序加载到设备时必要初始化
 */
drvstus_t drvtick_entry(driver_t *drvp,u64_t val,void* p);

/**
 * 驱动程序退出函数
 */
drvstus_t drvtick_exit(driver_t* drvp,u64_t val,void* p);


#endif //__DRVTICK_H