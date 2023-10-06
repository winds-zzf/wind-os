/**
 * 
 */
#include "globaltype.h"
#include "globalctrl.h"

void kvmarea_init(){
	return;
}


void kvmArea_init(KvmArea *area){
	spinlock_init(&area->lock);
	list_init(&area->hook);
	area->flag = 0;
	area->limits = 0;
	area->mapType = 0;
	area->start = 0;
	area->end = 0;
	area->father = NULL;
	area->box = NULL;
	
	return;
}


void kvmSpace_init(KvmSpace *space){
	spinlock_init(&space->lock);
	space->flag = 0;

	list_init(&space->areas);
	space->areasNum = 0;

	space->startArea= NULL;
	space->endArea = NULL;
	space->curArea = NULL;
	space->father = NULL;

	space->freeStart = 0;
	space->freeEnd = 0;

	space->private = NULL;
	space->extend = NULL;
	
	return;
}


KvmArea* new_kvmArea(){
	//创建一个内存对象
	KvmArea *area = (KvmArea*)mobj_new(sizeof(KvmArea));
	if(NULL==area){
		return NULL;
	}
	kvmArea_init(area);
	return area;
}


bool_t del_kvmArea(KvmArea* area){
	if(NULL==area){
		return FALSE;
	}
	return mobj_delete(area,sizeof(KvmArea));
}


KvmSpace* new_kvmSpace(){
	KvmSpace *space = (KvmSpace*)mobj_new(sizeof(KvmSpace));
	if(NULL==space){
		return NULL;
	}
	kvmSpace_init(space);
	return space;
}

bool_t del_kvmSpace(KvmSpace *space){
	if(NULL==space){
		return FALSE;
	}
	return mobj_delete(space,sizeof(KvmSpace));
}


/**
 * 初始化一个虚拟内存空间（因为现在还没有进程，所以这里直接生成一个测试用的实例变量）
 */
bool_t init_userspace_kvmspace(KvmSpace* space){
	kvmSpace_init(space);
	//第一个内存区间和栈区指针
	KvmArea *firstArea = NULL,*stackArea = NULL;
	//分配一个内存区和栈区
	firstArea = new_kvmArea();
	if(NULL != firstArea){	//申请成功
		stackArea = new_kvmArea();
		if(NULL == stackArea){	//申请失败
			del_kvmArea(firstArea);	//将已经申请的虚拟内存区域释放	
			return FALSE;
		}
		//申请成功
	}else return FALSE;
	//set fields
	firstArea->start = USER_VIRADR_START + 0x1000;
	firstArea->end = firstArea->start + 0x4000;
	firstArea->father = space;	//设置space为plainArea和stackArea的上层结构

	//为什么用户栈要设置这么大？
	stackArea->start = ALIGN_4K(USER_VIRADR_END - 0x40000000);
	stackArea->end = USER_VIRADR_END;
	stackArea->father = space;

	/**	
	 * 为什么这段代码需要对space结构上锁？？？
	 * space是会被多个线程同时访问吗？
	 */
	spinlock_lock(&space->lock);
	/**
	 * 为什么这里能分配的起止地址是整个用户虚拟地址空间？
	 */
	space->freeStart = USER_VIRADR_START;
	space->freeEnd = USER_VIRADR_END;
	
	//标记这个内存空间的第一个区间和最后一个区间
	space->startArea = firstArea;
	space->endArea = stackArea;

	//将两个区间挂入虚拟内存空间链表中
	list_add_tail(&firstArea->hook,&space->areas);	//使用尾插法保证排队顺序
	list_add_tail(&stackArea->hook,&space->areas);

	//区间数+2
	space->areasNum+=2;

	spinlock_unlock(&space->lock);
	
	return TRUE;
}

