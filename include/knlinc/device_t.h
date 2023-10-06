/**
 * 逻辑设备
 * 类型定义
 */
#ifndef __DEVICE_T_H
#define __DEVICE_T_H


/** 逻辑设备编号 **/
#define	NOT_DEVICE			0	//不表示任何设备
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
#define	RTC_DEVICE			22	//实时时钟设备
#define	SD_DEVICE				25	//SD卡设备
#define	AUDIO_DEVICE			26	//音频设备
#define	TOUCH_DEVICE			27	//触控设备
#define	NETWORK_DEVICE			28	//网络设备
#define	VIR_DEVICE			29	//虚拟设备
#define	FILESYS_DEVICE			30	//文件系统设备
#define	SYSTICK_DEVICE			31	//系统TICK设备
#define	UNKNOWN_DEVICE			32	//未知设备，也是设备
#define	HD_DEVICE				33	//硬盘设备


/* 设备ID */
typedef struct DEVID_T{
	u64_t	mtype;	//设备类型号
	u64_t	stype;	//设备子类型（子设备号用于解决多种相同设备）
	u64_t	id;		//设备序号
}devid_t;


/* 设备描述符 */
typedef struct DEVICE_T{
	List 	list;		//设备列表
	



}device_t;

/* 驱动描述符 */
typedef struct DRIVER_T{



}driver_t;


/** 
 * 同类设备链表
 * 每一类设备可能会有多个设备，因此创建一个链表来组织同类设备
 */
typedef struct DEVLST_T{



}devlst_t;


/* 设备表 */
typedef struct DEVTABLE_T{



}devtable_t;

/* 声明全局设备表 */
extern devtable_t devtable;


#endif //__DEVICE_T_H