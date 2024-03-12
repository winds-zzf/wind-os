/**
 * 文件系统
 * API定义
 */
#ifndef __DRVRFS_H
#define __DRVRFS_H


/**
 *
 */
drvstus_t drvrfs_entry(driver_t *drvp,u64_t val,void* p);

/**
 *
 */
drvstus_t drvrfs_exit(driver_t *drvp,u64_t val,void* p);


#endif //__DRVRFS_H