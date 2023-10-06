/**
 * 内核层聚集类型文件
 */

#ifndef __KNLTYPE_H
#define __KNLTYPE_H


//虚拟内存
#include "kvmbox_t.h"
#include "kvmarea_t.h"
#include "kvmprocess_t.h"
#include "mempool_t.h"

//进程&线程 管理
#include "thread_t.h"
#include "schedule_t.h"
#include "idle_t.h"
#include "waitlist_t.h"

//设备管理
#include "device_t.h"


#endif //__KNLTYPE_H