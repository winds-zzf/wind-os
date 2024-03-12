/**
 * time库函数
 */

#include "globaltype.h"
#include "globalctrl.h"

/**
 * 时间函数
 */
sysstus_t time(time_t *timep){
	return sys_time(timep);		//调用时间API
}
