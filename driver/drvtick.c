/**
 * 系统时钟模块
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

/**
 * 一个驱动程序必须包含接口规定的所有函数。在各个函数中可以返回一个错误状态，而不做任何实际工作
 * 但是必须要有这个函数
 */


/**
 * 打开设备函数
 */
drvstus_t tick_open(device_t* devp,void* iopack){
	//增加设备引用计数
	device_inc_devcount(devp);
	return DFCOKSTUS;
}
/**
 * 关闭设备函数
 */
drvstus_t tick_close(device_t* devp,void* iopack){
	//减少设备引用计数
	device_dec_devcount(devp);
	return DFCOKSTUS;
}
/**
 * 读设备数据函数
 */
drvstus_t tick_read(device_t* devp,void* iopack){
	return DFCERRSTUS;
}
/**
 * 写设备数据函数
 */
drvstus_t tick_write(device_t* devp,void* iopack){
	return DFCERRSTUS;
}
/**
 * 调整读写设备数据位置函数
 */
drvstus_t tick_lseek(device_t* devp,void* iopack){
	return DFCERRSTUS;
}
/**
 * 控制设备函数
 */
drvstus_t tick_ioctrl(device_t* devp,void* iopack){
	return DFCERRSTUS;
}
/**
 * 开启设备函数
 */
drvstus_t tick_dev_start(device_t* devp,void* iopack){
	return DFCERRSTUS;
}
/**
 * 关闭设备函数
 */
drvstus_t tick_dev_stop(device_t* devp,void* iopack){
	return DFCERRSTUS;
}
/**
 * 设置设备电源函数
 */
drvstus_t tick_set_powerstus(device_t* devp,void* iopack){
	return DFCERRSTUS;
}
/**
 * 枚举设备函数
 */
drvstus_t tick_enum_dev(device_t* devp,void* iopack){
	return DFCERRSTUS;
}
/**
 * 刷新设备缓存函数
 */
drvstus_t tick_flush(device_t* devp,void* iopack){
	return DFCERRSTUS;
}
/**
 * 设备关机函数
 */
drvstus_t tick_shutdown(device_t* devp,void* iopack){
	return DFCERRSTUS;
}
/**
 * 设备中断处理函数
 * 当时钟设备发出中断信号后，找到中断异常描述符，随后找到中断服务例程，调用中断回调函数，处理中断
 */
drvstus_t tick_handle(uint_t intnum,device_t* devp,void* sframe){
	//更新当前进程的时钟信息
	schedule_inc_tick();
	//更新系统时间
	systime_update();

	return DFCOKSTUS;
}

/**
 * 初始化8254定时器
 */
static void init_8254(){
	out_u8_p(PTIPROTM,TIMEMODE);
	out_u8_p(PTIPROT1,HZLL);
	out_u8_p(PTIPROT1,HZHH);
	
	return;
}

/**
 * 配置驱动程序信息
 */
static void set_driver(driver_t* drvp){
	//设置驱动程序功能派发函数
	drvp->dipfun[IOIF_CODE_OPEN] = tick_open;
	drvp->dipfun[IOIF_CODE_CLOSE] = tick_close;
	drvp->dipfun[IOIF_CODE_READ] = tick_read;
	drvp->dipfun[IOIF_CODE_WRITE] = tick_write;
	drvp->dipfun[IOIF_CODE_LSEEK] = tick_lseek;
	drvp->dipfun[IOIF_CODE_IOCTRL] = tick_ioctrl;
	drvp->dipfun[IOIF_CODE_DEV_START] = tick_dev_start;
	drvp->dipfun[IOIF_CODE_DEV_STOP] = tick_dev_stop;
	drvp->dipfun[IOIF_CODE_SET_POWERSTUS] = tick_set_powerstus;
	drvp->dipfun[IOIF_CODE_ENUM_DEV] = tick_enum_dev;
	drvp->dipfun[IOIF_CODE_FLUSH] = tick_flush;
	drvp->dipfun[IOIF_CODE_SHUTDOWN] = tick_shutdown;
	drvp->name = "tick0drv";	//设置驱动程序名称
	return;
}
/**
 * 配置设备信息
 */
static void set_device(device_t* devp,driver_t* drvp){
	devp->flag = DEVFLAG_SHARE;		//设备可共享访问
	devp->status = DEVSTUS_NORML;		//设备正常状态
	devp->id.mtype = SYSTICK_DEVICE;	//设备主类型，抽象类型之一
	devp->id.stype = 0;				//设备子类型
	devp->id.num = 0;				//设备号
	devp->name = "tick0";			//设备名
	
	return;
}


/**
 * 驱动程序入口函数
 * 入口函数执行驱动程序加载到设备时必要初始化
 */
drvstus_t drvtick_entry(driver_t *drvp,u64_t val,void* p){
	//需要保证内核给入口函数的驱动程序不为空
	if(NULL==drvp){
		return DFCERRSTUS;
	}
	
	//创建一个设备实例
	device_t* devp = device_new();
	if(NULL==devp){	//设备实例创建失败
		return DFCERRSTUS;
	}

	//配置驱动程序和设备信息
	set_driver(drvp);
	set_device(devp,drvp);

	//将设备挂载到驱动中
	if(device_add_driver(devp,drvp)==DFCERRSTUS){
		//释放资源
		if(device_delete(devp)==DFCERRSTUS){
			/**
			 * 为什么释放失败不死机？
			 */
			return DFCERRSTUS; 
		}
		return DFCERRSTUS;
	}

	//向内核注册设备
	if(device_register(devp)==DFCERRSTUS){
		if(device_delete(devp)==DFCERRSTUS){
			return DFCERRSTUS;
		}
		return DFCERRSTUS;
	}

	//安装中断回调函数(为什么是20？？？)
	if(install_callback(devp,tick_handle,0x20)==DFCERRSTUS){
		/**
		 * 为什么这里没有删除device_t描述符
		 */
		return DFCERRSTUS;
	}

	//初始化8254时钟物理设备
	init_8254();
	
	//开启8254硬件设备中断信号线
	if(hal_enable_intline(0)==DFCERRSTUS){
		/**
		 * 为什么这里没有删除device_t描述符
		 */
		return DFCERRSTUS;
	}
	
 	return DFCOKSTUS;
 }
 
/**
 * 驱动程序退出函数
 */
drvstus_t drvtick_exit(driver_t* drvp,u64_t val,void* p){
	return DFCERRSTUS;
}