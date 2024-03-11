/**
 * 时间模块
 * 系统调用API
 */

#include "globaltype.h"
#include "globalctrl.h"


/**
 * 系统调用
 */
sysstus_t sys_time(buf_t time){
	sysstus_t rets;
	//传递参数，使用软中断陷入内核态,在内核态查询内核服务表，选择响应的服务并执行
	API_ENTRY_PARE1(SERVICE_TIME,rets,time);
	
	return rets;
}
