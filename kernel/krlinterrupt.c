/**
 * 内核中断接口
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"


/**
 * 设备描述符
 * 中断回调函数
 * 物理中断线（中断号）
 */
intservice_t* add_irqhandle(device_t* devp,IntCallback handle,uint_t iline){
	
	//根据中断号获取中断异常描述符
	IntFault *ifault = get_intFault(iline);
	if(NULL==ifault){
		return NULL;
	}
	
	//申请一片内存存储中断服务实例
	intservice_t* iservice = (intservice_t*)mempool_new(sizeof(intservice_t));
	if(NULL==iservice){
		return NULL;
	}
	
	//初始化中断服务实例,将设备指针，回调函数，上层中断异常描述符填入其中
	intservice_t_init(iservice,0,ifault,devp,handle);
	//将中断服务例程挂载到到中断异常描述符
	if(add_intservice(ifault,iservice)==FALSE){
		//添加失败，释放创建的intservice_t实例
		if(mempool_delete((addr_t)iservice,sizeof(intservice_t)==FALSE)){
			{printk("error in add_irqhandle");die(0);}
		}
		return NULL;
	}
	
	return iservice;
}

/**
 * 
 */
drvstus_t enable_intline(uint_t intnum){
	return 1;
}

/**
 * 
 */
drvstus_t disable_intline(uint_t intnum){
	return 1; 
}
