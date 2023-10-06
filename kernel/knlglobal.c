/**
 * 内核层全局变量定义
 */
#include "globaltype.h"
#include "globalctrl.h"


KvmProcess kvmProcess;	//进程虚拟内存空间
scheclass_t scheclass;	//全局调度器结构
mempool_t	mempool;		//内存池结构
devtable_t devtable;	//设备表