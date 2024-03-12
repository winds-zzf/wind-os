/**
 * 固态硬盘驱动
 * API定义
 */
#ifndef __DRVHHD_H
#define __DRVHHD_H


/**
 *
 */
drvstus_t drvhhd_entry(driver_t* drvp,u64_t val,void* p);

/**
 *
 */
drvstus_t drvhhd_exit(driver_t* drvp,u64_t val,void* p);


#endif //__DRVHHD_H