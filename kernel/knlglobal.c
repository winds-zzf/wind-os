/**
 * 内核层全局变量定义
 */
#include "globaltype.h"
#include "globalctrl.h"


KvmProcess kvmProcess;	//进程虚拟内存空间
scheclass_t scheclass;	//全局调度器结构
mempool_t	mempool;		//内存池结构
devtable_t devtable;	//设备表
systime_t systime;		//系统时间


/**
 * 驱动程序在内核程序编译后需要链接一起。因为加载程序不仅仅是把驱动程序放在内存中就可以了，
 * 还要进行程序链接相关的操作，这个过程很复杂。这里直接在编译内核的时候就进行链接，也就是使用驱动程序表
 * 相当于是一种静态的链接
 */
drventryexit_t drventries[] = {
		drvtick_entry,
		drvrfs_entry,
		drvkeyboard_entry,
		//drvhhd_entry,
		NULL,
	};


/**
 * 系统服务表
 * 
 * 
 */
syscall_t osservicetab[SERVICE_MAX] = {
		NULL,					//保留
		sve_time,					//获取时间服务接口
		//krlsvetabl_mallocblk,		//内存分配服务接口
		//krlsvetabl_mfreeblok,		//内存释放服务接口
		//krlsvetabl_exel_thread,	//进程服务接口
		//krlsvetabl_exit_thread,	//进程退出服务接口
		//krlsvetabl_retn_threadhand,	//获取进程id服务接口
		//krlsvetabl_retn_threadstus,	//获取进程状态服务接口
		//krlsvetabl_set_threadstus,	//设置进程状态服务接口
		//krlsvetabl_open,			//文件打开服务接口
		//krlsvetabl_close,			//文件关闭服务接口
		//krlsvetabl_read,			//文件读取服务接口
		//krlsvetabl_write,			//文件写入服务接口
		//krlsvetabl_ioctrl,		//文件控制服务接口
		//krlsvetabl_lseek,			//文件随机读写服务接口
		//krlsvetabl_time,
	};
