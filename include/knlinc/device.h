/**
 * 操作系统设备抽象模块
 * API定义
 */
#ifndef __DEVICE_H
#define __DEVICE_H

#include "device_t.h"

/**
 * 初始化设备
 */
void device_init();


/**
 * 初始化驱动程序
 */
void driver_init();


/**
 * 新建一个driver_t实例变量
 */
driver_t* driver_new();


/**
 * 释放一个driver_t实例变量
 */
bool_t driver_delete(driver_t* driver);


/**
 * 新建一个device_t示例变量
 */
device_t* device_new();


/**
 * 释放一个device_t示例变量
 */
bool_t device_delete(device_t* device);


/**
 * 设备使用计数+1
 */
drvstus_t device_inc_devcount(device_t* devp);


/**
 * 设备使用计数-1
 */
drvstus_t device_dec_devcount(device_t* devp);


/**
 * 将设备挂载到驱动
 */
drvstus_t device_add_driver(device_t* devp,driver_t* drvp);

/**
 * 向内核注册设备
 */
drvstus_t device_register(device_t* device);

/**
 * 安装中断回调函数接口
 * handle:设备中断回调函数入口
 * phyiline:设备中断信号线(对应中断号)
 */
drvstus_t install_callback(device_t* devp,IntCallback handle,uint_t phyiline);


#endif //__DEVICE_H