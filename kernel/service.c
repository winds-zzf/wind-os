/**
 * 系统服务模块
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

/**
 * 系统服务分发器函数
 * 由中断处理的框架函数调用
 */
sysstus_t krlservice(uint_t inr,void* sframe){
	//检查系统服务号是否合法
	if(SERVICE_MAX<=inr){
		return SYSSTUSERR;
	}
	//检查是否有服务接口函数
	if(NULL == osservicetab[inr]){
		//系统服务不存在
		return SYSSTUSERR;
	}
	//调用对应的服务接口函数
	return osservicetab[inr](inr,(sframe_t*)sframe); 
}
