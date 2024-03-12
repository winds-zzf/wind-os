/**
 * 逻辑设备管理模块
 */
#include "globaltype.h"
#include "globalctrl.h"

/**
 * 初始化设备id
 */
static void devid_t_init(devid_t* devid){
	devid->mtype = 0;
	devid->stype = 0;
	devid->num = 0;
	
	return;
}

/**
 * 初始化设备描述符
 */
static void device_t_init(device_t* device){
	spinlock_init(&device->lock);
	device->status = 0;
	device->flag = 0;
	device->count = 0;
	sem_t_init(&device->sem);
	devid_t_init(&device->id);
	
	list_t_init(&device->hook);
	list_t_init(&device->devhook);
	list_t_init(&device->drvhook);

	list_t_init(&device->intserlst);
	device->intsernum = 0;
	list_t_init(&device->reqlst);
	device->reqnum = 0;
	sem_t_init(&device->waitints);

	device->drv = NULL;
	device->name = NULL;

	device->attr = NULL;
	device->privdata = NULL;
	device->userdata = NULL;
	device->extdata = NULL;
	
	return;
}

/**
 * 初始化设备驱动描述符
 */
static void driver_t_init(driver_t* driver){
	spinlock_init(&driver->lock);
	list_t_init(&driver->hook);

	driver->status = 0;
	driver->flag = 0;
	driver->id = (u64_t)driver;	//将驱动描述符地址作为驱动器id
	driver->count = 0;

	sem_t_init(&driver->sem);
	driver->safe = NULL;
	driver->attr = NULL;
	driver->privdata = NULL;
	list_t_init(&driver->alldevlst);

	for(uint_t i=0;i<IOIF_CODE_MAX;i++){
		driver->dipfun[i] = NULL;
	}
	driver->entry = NULL;
	driver->exit = NULL;
	
	driver->userdata = NULL;
	driver->extdata = NULL;
	
	driver->name = NULL;
	
	return;
}

/**
 * 初始化设备链表
 */
static void devlst_t_init(devlst_t* list,uint_t type){
	list->type = type;
	list_t_init(&list->devlst);
	list->devNum = 0;
	
	return;
}

/**
 * 初始化设备表
 */
static void devtable_t_init(devtable_t* table){
	spinlock_init(&table->lock);
	list_t_init(&table->hook);
	list_t_init(&table->devlst);
	list_t_init(&table->drvlst);
	table->devNum = 0;
	table->drvNum = 0;
	//初始化设备链表
	for(uint_t i=0;i<DEVICE_MAX;i++){
		devlst_t_init(&table->devlsts[i],i);
	}
	return;
}


void device_init(){
	//初始化全局设备表，以便进行设备管理
	devtable_t_init(&devtable);
	return;
}


/**
 * 新建一个driver_t实例变量
 */
driver_t* driver_new(){
	driver_t* driver = (driver_t*)mempool_new(sizeof(driver_t));
	if(NULL==driver){
		return NULL;
	}
	driver_t_init(driver);
	return driver;
}

/**
 * 释放一个driver_t实例变量
 */
bool_t driver_delete(driver_t* driver){
	if(mempool_delete((addr_t)driver,sizeof(driver_t))==FALSE){
		{printk("delete_driver error");die(0);}
		return FALSE;
	}
	return TRUE;
}

/**
 * 新建一个device_t示例变量
 */
device_t* device_new(){
	device_t* device = (device_t*)mempool_new(sizeof(device_t));
	if(NULL==device){
		return NULL;
	}
	device_t_init(device);
	return device;
}

/**
 * 释放一个device_t示例变量
 */
bool_t device_delete(device_t* device){
	if(mempool_delete((addr_t)device,sizeof(device_t))==FALSE){
		error("delete_driver error");
		return FALSE;
	}
	return TRUE;
}


/**
 * 驱动入口函数就是一个用于将驱动程序加载到内核的标准函数
 * 用于初始化一个驱动程序，初始化该驱动程序的各种信息
 */


/**
 * 将驱动程序添加到内核
 */
static drvstus_t driver_add_system(driver_t* driver){
	devtable_t* table = &devtable;	//获取全局设备表

	//全局描述符表上锁并关中断
	cpuflag_t cpuflag;
	spinlock_lock_cli(&table->lock,&cpuflag);

	//将驱动程序挂载到全局驱动程序链表上，驱动程序计数+1
	list_add(&driver->hook,&table->drvlst);
	table->drvNum++;
	
	spinlock_unlock_sti(&table->lock,&cpuflag);
	return DFCOKSTUS;
}


/**
 * 运行驱动程序提供的入口程序
 */
static drvstus_t run_drventry(drventryexit_t entry){
	//1.创建一个驱动程序实例
	driver_t* driver = driver_new();

	if(NULL==driver){
		return DFCERRSTUS;
	}

	//2.运行驱动程序入口函数（由硬件厂商提供的标准化程序）:初始化驱动描述符
	if(entry(driver,0,NULL)==DFCERRSTUS){
		return DFCERRSTUS;
	}

	//3.把驱动程序加入内核
	if(driver_add_system(driver)==DFCERRSTUS){
		return DFCERRSTUS;
	}

	//驱动处理完毕
	return DFCOKSTUS;
}

void driver_init(){
	//遍历驱动程序表中的每个驱动程序入口函数
	for(uint_t i=0;drventries[i]!=NULL;i++){
		//运行驱动程序的入口函数，进行驱动程序的初始化
		if(run_drventry(drventries[i])==DFCERRSTUS){
			error("driver_init error\n");		//出错
		}//运行成功
	}
	
	return;
}

/**
 * 以下是供驱动程序开发者使用的接口函数
 */

/**
 * 内核分配设备id
 * 编号局限于同组设备：找到同组设备链表，确定组内最大编号，将最大编号+1最为新分配编号
 */
static drvstus_t devid_allocate(device_t* devp){
	drvstus_t rets = DFCERRSTUS;
	devtable_t* table = &devtable;
	u64_t mtype = devp->id.mtype;		//获取抽象类型
	//检查设备主编号是否合法
	if(mtype>DEVICE_MAX){
		return DFCERRSTUS;
	}

	cpuflag_t cpuflag;
	spinlock_lock_cli(&devp->lock,&cpuflag);
	//1.检查抽象编号对应的设备类型是否一致
	if(mtype!=table->devlsts[mtype].type){
		rets = DFCERRSTUS;
		goto ret_step;
	}
	//2.遍历同类设备链表，获取最大编号
	u64_t maxnum = 0;
	list_t* pos = NULL;
	device_t* dev;
	list_for_each(pos,&table->devlsts[mtype].devlst){
		dev = list_entry(pos,device_t,hook);
		if(dev->id.num>maxnum){
			maxnum = dev->id.num;
		}
	}
	//3.取组内最大num加1作为新分配num
	devp->id.num = maxnum+1;
	rets = DFCOKSTUS;

ret_step:
	spinlock_unlock_sti(&devp->lock,&cpuflag);
	return rets;
}

/**
 * 比较设备ID是否相等
 */
static bool_t devid_equal(devid_t* a,devid_t* b){
	if(a->mtype!=b->mtype){
		return FALSE;
	}
	if(a->stype!=b->stype){
		return FALSE;
	}
	if(a->num!=b->num){
		return FALSE;
	}
	
	return TRUE;
}

/**
 * 将设备挂载到驱动
 */
drvstus_t device_add_driver(device_t* devp,driver_t* drvp){
	list_t* pos = NULL;
	device_t *dev;
	//遍历这个驱动程序上的设备链表，检查是否有重复的设备id
	list_for_each(pos,&drvp->alldevlst){
		dev = list_entry(pos,device_t,hook);
		//检查：如果id重复，则返回错误
		if(devid_equal(&dev->id,&devp->id)==TRUE){
			return DFCERRSTUS;
		}
	}

	//将设备挂载到链表上，不计数
	list_add(&devp->hook,&drvp->alldevlst);
	devp->drv = drvp;		//让设备的drv字段指管理自己的那个驱动
	
	return DFCOKSTUS;
}

/**
 * 向内核注册设备
 * 
 */
drvstus_t device_register(device_t* devp){
	devtable_t *table = &devtable;	//获取设备表
	drvstus_t rets = DFCERRSTUS;

	cpuflag_t cpuflag = 0;
	spinlock_lock_cli(&table->lock,&cpuflag);
	uint_t mtype = devp->id.mtype;		//获取主类型
	
	//遍历全局设备链表，检查待注册设备id是否已经存在
	list_t *pos = NULL;
	device_t *dev = NULL;

	list_for_each(pos,&table->devlst){
		dev = list_entry(pos,device_t,hook);
		//比较待注册设备id和已存在设备id
		if(devid_equal(&dev->id,&devp->id)==TRUE){
			rets = DFCERRSTUS;
			goto ret_step;
		}
	}

	//将设备挂载到全局设备链表中
	list_add(&devp->hook,&table->devlst);
	table->devNum++;

	//将设备添加到对应设备类型的链表中
	list_add(&devp->devhook,&table->devlsts[mtype].devlst);
	table->devlsts[mtype].devNum++;
	
	rets = DFCOKSTUS;
	
ret_step:

	spinlock_unlock_sti(&table->lock,&cpuflag);
	return rets;
}

/**
 * 安装中断回调函数接口
 * handle:设备中断回调函数入口
 * phyiline:设备中断信号线(对应中断号)
 */
drvstus_t install_callback(device_t* devp,IntCallback handle,uint_t phyiline){
	//调用内核层中断框架接口函数，将中断回调函数挂载到中断异常描述符中,返回对应的中断服务描述符
	intservice_t *iservice = add_irqhandle(devp,handle,phyiline);
	if(NULL==iservice){
		return DFCERRSTUS;
	}
	cpuflag_t cpuflag;
	spinlock_lock_cli(&devp->lock,&cpuflag);

	//中断服务例程结构挂载到这个设备驱动中
	list_add(&iservice->devhook,&devp->intserlst);
	devp->intsernum++;	//设备中断服务例程数+1	
	
	spinlock_unlock_sti(&devp->lock,&cpuflag);

	return DFCOKSTUS;
}


/**
 * 调用设备驱动程序
 * devp:
 * iocode:
 * val1:
 * val2:
 * p1:
 * p2:
 */
drvstus_t device_call_driver(device_t* devp,u64_t iocode,u64_t val1,u64_t val2,void* p1,void* p2){
	//参数检查
	if(NULL==devp || iocode>=IOIF_CODE_MAX){
		return DFCERRSTUS;
	}

	//获取设备相关的驱动
	driver_t *drvp = devp->drv;
	if(NULL==drvp){
		return DFCERRSTUS;
	}

	//根据IO操作码作为索引，调用驱动分派函数中的指定驱动函数
	return drvp->dipfun[iocode](devp,p2);
}

/**
 * 发送设备IO
 * node:对象节点
 */
drvstus_t device_io(objnode_t* node){
	//获取设备对象
	device_t *devp = (device_t*)(node->objadr);
	//检查操作对象类型是不是文件或设备类型，对象地址是否为空
	if((OBJNODE_TYPE_DEVICE==node->objtype && OBJNODE_TYPE_FILE!=node->objtype) || NULL==node->objadr){
		return DFCERRSTUS;
	}

	//调用驱动函数
	return device_call_driver(devp,node->opcode,0,0,NULL,node);
}


/**
 * 增加设备引用计数
 */
drvstus_t device_inc_devcount(device_t* devp){	
	cpuflag_t cpuflag;
	spinlock_lock_cli(&devp->lock,&cpuflag);
	devp->count++;
	spinlock_unlock_sti(&devp->lock,&cpuflag);
	return DFCOKSTUS;
}

/**
 * 减少设备引用计数
 */
drvstus_t device_dec_devcount(device_t* devp){
	//参数检查
	if(devp->count<1){
		return DFCERRSTUS;
	}
	
	cpuflag_t cpuflag;
	spinlock_lock_cli(&devp->lock,&cpuflag);	
	devp->count--;
	spinlock_unlock_sti(&devp->lock,&cpuflag);
	return DFCOKSTUS;
}

