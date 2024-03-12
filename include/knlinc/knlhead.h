/**
 * kernel层聚集头文件
 */
#ifndef __KNLHEAD_H
#define __KNLHEAD_H


#include "kernel.h"

//虚拟内存
#include "kvmarea.h"		//虚拟内存区间和虚拟内存空间
#include "kvmbox.h"			//页面盒子和盒子头
#include "kvmprocess.h"		//进程内存空间
#include "mempool.h"

//进程&线程 模块
#include "objnode.h" 
#include "thread.h"
#include "schedule.h"
#include "idle.h"
#include "waitlist.h"
#include "sem.h"

//设备管理
#include "device.h"
#include "krlinterrupt.h"

//文件管理


//服务接口
#include "syscall.h"
#include "service.h"


//时间管理
#include "systime.h"


#endif //__KNLHEAD_H