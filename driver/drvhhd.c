/**
 * 固态硬盘驱动程序
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"


drvstus_t hhd_open(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t hhd_close(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t hhd_read(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t hhd_write(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t hhd_lseek(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t hhd_ioctrl(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t hhd_dev_start(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t hhd_dev_stop(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t hhd_set_powerstus(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t hhd_enum_dev(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t hhd_flush(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t hhd_shutdown(device_t* devp,void* iopack){
	return DFCERRSTUS;
}



static drvstus_t set_driver(driver_t* drvp){
	//驱动接口函数表
	drvp->dipfun[IOIF_CODE_OPEN] = hhd_open;
	drvp->dipfun[IOIF_CODE_CLOSE] = hhd_close;
	drvp->dipfun[IOIF_CODE_READ] = hhd_read;
	drvp->dipfun[IOIF_CODE_WRITE] = hhd_write;
	drvp->dipfun[IOIF_CODE_LSEEK] = hhd_lseek;
	drvp->dipfun[IOIF_CODE_IOCTRL] = hhd_ioctrl;
	drvp->dipfun[IOIF_CODE_DEV_START] = hhd_dev_start;
	drvp->dipfun[IOIF_CODE_DEV_STOP] = hhd_dev_stop;
	drvp->dipfun[IOIF_CODE_SET_POWERSTUS] = hhd_set_powerstus;
	drvp->dipfun[IOIF_CODE_ENUM_DEV] = hhd_enum_dev;
	drvp->dipfun[IOIF_CODE_FLUSH] = hhd_flush;
	drvp->dipfun[IOIF_CODE_SHUTDOWN] = hhd_shutdown;
	//驱动程序名
	
	return DFCOKSTUS;
}

static drvstus_t set_device(device_t* devp,driver_t* drvp){

	//设备文件名
	
	return DFCOKSTUS;
}

drvstus_t hhd_handle(uint_t intnum,device_t* devp,void* sframe){


	
	return DFCOKSTUS;
}

drvstus_t drvhhd_entry(driver_t* drvp,u64_t val,void* p){
	if(NULL==drvp){
		return DFCERRSTUS;
	}

	//分配一个设备结构实例
	device_t* devp = device_new();
	if(NULL==devp){
		return DFCERRSTUS;
	}

	//配置设备和驱动信息
	set_driver(drvp);
	set_device(devp,drvp);

	//将设备添加到驱动
	if(device_add_driver(devp,drvp)==DFCERRSTUS){
		if(device_delete(devp)==DFCERRSTUS){
			return DFCERRSTUS;
		}
		return DFCERRSTUS;
	}

	//向驱动注册设备
	if(device_register(devp)==DFCERRSTUS){
		if(device_delete(devp)==DFCERRSTUS){
			return DFCERRSTUS;
		}
		return DFCERRSTUS;		
	}
	//安装中断回调函数
	/*
	if(install_callback(devp,hhd_handle,0x34)==DFCERRSTUS){
		return DFCERRSTUS;
	}
	*/

	/**
	 * 从硬盘读取数据是通过中断方式是IO方式还是通过DMA中断控制器？？？
	 */

	//开启设备的中断信号线
	
	return DFCOKSTUS;
}

drvstus_t drvhhd_exit(driver_t* drvp,u64_t val,void* p){


	
	return DFCERRSTUS;
}



