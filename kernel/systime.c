/**
 * 内核时间模块
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"


static void systime_t_init(systime_t* time){
	spinlock_init(&time->lock);
	time->year = 0;
	time->month = 0;
	time->day = 0;
	time->date = 0;
	time->hour = 0;
	time->minute = 0;
	time->second = 0;
	time->data = NULL;

	return;
}


/**
 * 初始化系统时间
 */
void systime_init(){
	systime_t_init(&systime);
	
	return ;
}

/**
 * 更新系统时间
 */
void systime_update(){
	systime_t *time = &systime;
	u8_t tmp;

	cpuflag_t cpuflag;
	/**
	 * 在访问CMOS时钟时，必须进制所有中断(包括不可屏蔽中断
	 */
	spinlock_lock_cli(&time->lock,&cpuflag);
	CMOS_READ(tmp,CMOS_ADR_SECOND);
	time->second = BCD_TO_BIN(tmp);
	CMOS_READ(tmp,CMOS_ADR_MINUTE);
	time->minute = BCD_TO_BIN(tmp);
	CMOS_READ(tmp,CMOS_ADR_HOUR);
	time->hour = BCD_TO_BIN(tmp);
	CMOS_READ(tmp,CMOS_ADR_DAY);
	time->day = BCD_TO_BIN(tmp);
	CMOS_READ(tmp,CMOS_ADR_MONTH);
	time->month = BCD_TO_BIN(tmp);
	/**
	 * 这里tmp是u8_t,获取的是BCD码，因此年份只能表示0-99
	 */
	CMOS_READ(tmp,CMOS_ADR_YEAR);
	time->year = 2000+BCD_TO_BIN(tmp);	
	spinlock_unlock_sti(&time->lock,&cpuflag);
	
	return;
}


/**
 * 设定系统时间
 */
void systime_set(uint_t year,uint_t month,uint_t day,uint_t date,uint_t hour,uint_t min,uint_t sec){



	
	return;
}

/**
 * 输出日期信息
 */
void systime_tostring(systime_t* time){
	printk("%d-%d-%d %d:%d:%d\n",time->year,time->month,time->day,time->hour,time->minute,time->second);
	return;
}


/**
 * 获取系统时间
 */
static sysstus_t systime_copy(time_t *time){
	if(NULL==time){ //对参数进行判断
		return SYSSTUSERR;
	}
	systime_t* stime = &systime;	//操作系统保存时间的结构
	
	cpuflag_t cpuflag;
	spinlock_lock_cli(&stime->lock,&cpuflag);	//加锁
	//从系统时间数据拷贝副本到time结构中
	time->year = stime->year;
	time->month = stime->month;
	time->day = stime->day;
	time->date = stime->date;
	time->hour = stime->hour;
	time->minute = stime->minute;
	time->second = stime->second;
	spinlock_unlock_sti(&stime->lock,&cpuflag);
	return SYSSTUSOK;
}

/**
 * 系统调用服务例程
 */
sysstus_t sve_time(uint_t inr,sframe_t* sframe){
	if(inr!=SERVICE_TIME){	//判断是否是时间服务号
		return SYSSTUSERR;
	}
	//调用真正的时间服务函数
	return systime_copy((time_t*)sframe->parm1);
}
