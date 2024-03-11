/**
 * 键盘驱动程序模块
 * API定义
 */
#ifndef __DRVKEYBOARD_H
#define __DRVKEYBOARD_H


/**
 *
 */
drvstus_t drvkeyboard_entry(driver_t* drvp,u64_t val,void* p);

/**
 * 
 */
drvstus_t drvkeyboard_exit(driver_t* drvp,u64_t val,void* p);


#endif //__DRVKEYBOARD_H
