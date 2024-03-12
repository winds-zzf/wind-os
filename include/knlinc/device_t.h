/**
 * 逻辑设备
 * 类型定义
 */
#ifndef __DEVICE_T_H
#define __DEVICE_T_H


/**
 * 逻辑设备编号
 * 抽象设备类型
 */
#define	NOT_DEVICE			0	//不表示任何设备
#define	AMBABUS_DEVICE			1	//
#define	AHBBUS_DEVICE			2	//
#define	APBBUS_DEVICE			3	//
#define	BRIDGE_DEVICE			4	//总线桥接器设备
#define	CPUCORE_DEVICE			5	//CPU设备，CPU也是设备
#define	RAMCONTER_DEVICE		6	//内存控制器设备
#define	RAM_DEVICE			7	//内存设备
#define	USBHOSTCONTER_DEVICE	8	//USB主控制设备
#define	INTUPTCONTER_DEVICE		9	//中断控制器设备
#define	DMA_DEVICE			10	//DMA设备
#define	CLOCKPOWER_DEVICE		11	//时钟电源设备
#define	LCDCONTER_DEVICE		12	//LCD控制器设备
#define	NANDFLASH_DEVICE		13	//nandflash设备
#define	CAMERA_DEVICE			14	//摄像头设备
#define	UART_DEVICE			15	//串口设备
#define	TIMER_DEVICE			16	//定时器设备
#define	USB_DEVICE			17	//USB设备
#define	WATCHDOG_DEVICE		18	//看门狗设备
#define	IIC_DEVICE			19	//
#define	IIS_DEVICE			20	//
#define	GPIO_DEVICE			21	//
#define	RTC_DEVICE			22	//实时时钟设备
#define	SD_DEVICE				25	//SD卡设备
#define	AUDIO_DEVICE			26	//音频设备
#define	TOUCH_DEVICE			27	//触控设备
#define	NETWORK_DEVICE			28	//网络设备
#define	VIR_DEVICE			29	//虚拟设备
#define	FILESYS_DEVICE			30	//文件系统设备
#define	SYSTICK_DEVICE			31	//系统TICK设备
#define	UNKNOWN_DEVICE			32	//未知设备，也是设备

#define	DEVICE_MAX 33		//共定义了33种设备类型

/* 设备标志 */
#define DEVFLAG_EXELU (1<<0)
#define DEVFLAG_SHARE (1<<1)

/* 设备状态 */
#define DEVSTUS_NORML (1<<0)
#define DEVSTUS_FAIUL (1<<1)


typedef struct DRIVER_T driver_t;	//声明结构体DRIVER_T为driver_t

/* 设备ID */
typedef struct DEVID_T{
	uint_t	mtype;	//设备主类型号(master type):抽象设备类型
	uint_t	stype;	//设备子类型(sub type)（子设备号用于解决多种相同设备）:具体设备类型
	u64_t	num;		//设备号
}devid_t;

/** 
 * 设备描述符
 * 设备描述符需要挂载在三个不同的链表上
 *  1.全局设备链表
 *  2.设备分类链表
 *  3.驱动管理发的设备链表
 */
typedef struct DEVICE_T{
	spinlock_t	lock;	//保护设备描述符的自旋锁
	uint_t		status;	//设备状态
	uint_t		flag;	//设备标志
	uint_t 		count;	//设备引用计数，统计有多少个进程打开了这个设备
	sem_t		sem;		//设备信号量
	devid_t		id;		//设备id
	//三个钩子
	list_t		hook;	//钩子：挂载到全局设备链表
	list_t		devhook;	//钩子：挂载到分类设备链表
	list_t		drvhook;	//钩子：挂载到设备驱动程序

	list_t		intserlst;//设备中断服务例程
	uint_t		intsernum;//设备中断服务例程数量
	
	list_t		reqlst;	//对设备的请求服务链表
	uint_t		reqnum;	//对设备请求服务个数
	
	sem_t		waitints;	//用于等待设备的信号量

	driver_t*		drv;		//设备对应的驱动程序指针（用于访问设备时调用设备驱动程序的）
	char_t*		name;	//设备名
	//其他
	void*		attr;	//设备属性指针
	void*		privdata;	//设备私有数据指针
	void*		userdata;	//将来拓展所用
	void*		extdata;	//将来拓展所用
}device_t;

/**
 * 驱动程序的几种主要函数
 * iopack:I/O包
//驱动程序入口和退出函数
drvstus_t device_entry(driver_t* drvp,u64_t val,void* p);
drvstus_t device_exit(driver_t* drvp,u64_t val,void* p);
//设备中断处理函数
drvstus_t device_handle(uint_t ift_nr,void* devp,void* sframe);
//打开、关闭设备函数
drvstus_t	device_open(device_t* devp,void* iopack);
drvstus_t device_close(device_t* devp,void* iopack);
//读、写设备数据函数
drvstus_t device_read(device_t* devp,void* iopack);
drvstus_t device_write(device_t* devp,void* iopack);
//调整读写设备数据位置函数
drvstus_t device_lseek(device_t* devp,void* iopack);
//控制设备函数
drvstus_t device_ioctrl(device_t* devp,void* iopack);
//开启、停止设备函数
drvstus_t device_dev_start(device_t* devp,void* iopack);
drvstus_t device_dev_stop(device_t* devp,void* iopack);
//设置设备电源函数
drvstus_t device_set_powerstus(device_t* devp,void* iopack);
//枚举设备函数
drvstus_t device_enum_dev(device_t* devp,void* iopack);
//刷新设备缓存函数
drvstus_t device_flush(device_t* devp,void* iopack);
//设备关机函数
drvstus_t device_shutdown(device_t* devp,void* iopack);
**/

/* 驱动程序功能码 */
#define IOIF_CODE_OPEN 			0	//对应于open操作
#define IOIF_CODE_CLOSE			1	//对应于close操作
#define IOIF_CODE_READ			2	//对应于read操作
#define IOIF_CODE_WRITE			3	//对应于write操作
#define IOIF_CODE_LSEEK			4	//对应于lseek操作
#define IOIF_CODE_IOCTRL			5	//对应于ioctrl操作
#define IOIF_CODE_DEV_START		6	//对应于dev_start操作
#define IOIF_CODE_DEV_STOP		7	//对应于dev_stop操作
#define IOIF_CODE_SET_POWERSTUS	8	//对应于set_powerstus操作
#define IOIF_CODE_ENUM_DEV		9	//对应于enum_dev操作
#define IOIF_CODE_FLUSH			10	//对应于flush操作
#define IOIF_CODE_SHUTDOWN		11	//对应于shutdown操作
#define IOIF_CODE_MAX			12	//对应于最大功能码数

//驱动程序分派函数指针类型
typedef drvstus_t (*drvcallfunc_t)(device_t*,void*);
//驱动程序入口、退出函数指针类型
typedef drvstus_t (*drventryexit_t)(driver_t*,u64_t,void*);

/* 驱动程序描述符 */
typedef struct DRIVER_T{
	spinlock_t	lock;	//保护驱动程序结构的自旋锁
	list_t		hook;	//挂载驱动程序的钩子
	
	u64_t	status;	//驱动程序相关状态
	u64_t	flag;	//驱动程序相关的标志
	u64_t	id;		//驱动程序id
	u64_t	count;	//驱动程序计数器
	sem_t	sem;		//驱动程序的信号量

	list_t	alldevlst;//挂载驱动程序所管理的所有设备的链表
	
	drvcallfunc_t	dipfun[IOIF_CODE_MAX];	//驱动程序功能派发函数指针数组
	drventryexit_t	entry;	//驱动程序的入口函数指针
	drventryexit_t	exit;	//驱动程序的退出函数指针
	char_t*	name;		//驱动程序名字

	void*	safe;		//驱动程序的安全体
	void*	attr;		//内核要求的驱动程序属性体
	void*	privdata;		//驱动程序私有数据指针
	void*	userdata;		//将来用于拓展
	void*	extdata;		//将来用于拓展
	
}driver_t;
/* 声明全局驱动程序表 */
extern drventryexit_t drventries[];


/** 
 * 同类设备链表
 * 每一类设备可能会有多个设备，因此创建一个链表来组织同类设备
 */
typedef struct DEVLST_T{
	uint_t 	type;	//挂载的设备类型
	list_t	devlst;	//挂载设备的链表
	uint_t	devNum;	//挂载设备数
}devlst_t;


/**
 * 设备表
 * 在设备表中可以找到所有的设备和所有的驱动
 */
typedef struct DEVTABLE_T{
	spinlock_t	lock;		//保护全局设备表的自旋锁
	list_t		hook;		//挂载全局设备表钩子的钩子
	list_t		devlst;		//全局设备链表
	list_t		drvlst;		//全局驱动程序链表，驱动程序不需要分类，一个链表就行
	uint_t		devNum;		//全局设备计数
	uint_t		drvNum;		//全局驱动程序计数
	//分类存放设备数据结构的devlst_t结构数组 
	devlst_t		devlsts[DEVICE_MAX];	//设备分组管理，同类设备分为一组
}devtable_t;


/* 声明全局设备表 */
extern devtable_t devtable;

#define DFCERRSTUS 0
#define DFCOKSTUS 1


#endif //__DEVICE_T_H