/**
 * 内核层中断框架模块
 * API定义
 */
#ifndef __KRLINTERRUPT_H
#define __KRLINTERRUPT_H

#include "device_t.h"


/**
 * 添加中断回调函数，并获取中断服务例程
 */
intservice_t* add_irqhandle(device_t* devp,IntCallback handle,uint_t iline);


/**
 * 开启一个中断源上的中断
 */
drvstus_t enable_intline(uint_t intnum);

/**
 * 屏蔽一个中断源上的中断
 */
drvstus_t disable_intline(uint_t intnum);


#endif //__KRLINTERRUPT_H