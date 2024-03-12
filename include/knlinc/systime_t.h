/**
 * 时间模块
 * 类型定义
 */
#ifndef __KRLTIME_T_H
#define __KRLTIME_T_H


/**
 * CMOS即Complementary Metal-Oxide-Semiconductor芯片 
 * CMOS芯片是计算机中用于存储系统实时时钟和BIOS设置的重要组件。
 * 它通常包含一个64字节的RAM区域，用于存储各种系统信息，如日期、时间、硬件配置等
 */

/**
 * 内核时间结构体
 */
typedef struct KTIME_T{
	spinlock_t	lock;	//自旋锁
	uint_t	year;
	uint_t	month;
	uint_t	day;
	uint_t	date;	//这个字段有用吗？？？
	uint_t	hour;
	uint_t	minute;
	uint_t	second;
	void*	data;
}systime_t;

/* 声明系统时间 */
extern systime_t systime;

/**
 * 用户时间结构体
 */
typedef struct TIME_T{
	uint_t	year;
	uint_t	month;
	uint_t	day;
	uint_t	date;	//???
	uint_t	hour;
	uint_t	minute;
	uint_t	second;
}time_t;

/**
 * 在访问CMOS时钟时，端口70h用于选择使用哪个寄存器，然后通过端口0x71读写这个寄存器
 */
#define CMOS_PORT_ADR	0x70
#define CMOS_PORT_DATA	0x71

#define CMOS_ADR_SECOND	0x00
#define CMOS_ADR_MINUTE	0x02
#define CMOS_ADR_HOUR	0x04
#define CMOS_ADR_DAY	0x07
#define CMOS_ADR_MONTH	0x08
#define CMOS_ADR_YEAR	0x09

/* 用于读取时间的宏 */
#define CMOS_READ(val,adr) ({out_u8(CMOS_PORT_ADR,adr);val=in_u8(CMOS_PORT_DATA);})
#define BCD_TO_BIN(bcd) ( ((bcd)&0xf) + ((bcd)>>4)*10 )

#endif //__KRLTIME_T_H