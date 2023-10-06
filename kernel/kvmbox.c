/**
 * 
 */
#include "globaltype.h"
#include "globalctrl.h"


void kvmBox_init(KvmBox *box){
	spinlock_init(&box->lock);
	list_init(&box->hook);

	box->status = 0;
	box->flag = 0;
	box->type = 0;
	
	//共享计数的初始化待定
	
	
	list_init(&box->pages);
	box->pagesNum = 0;

	box->father = NULL;
	box->fileNode = NULL;
	box->pager = NULL;
	
	box->extend = NULL;

	return;
}


void kvmBoxs_init(KvmBoxs *boxs){
	spinlock_init(&boxs->lock);
	list_init(&boxs->hook);
	
	boxs->status = 0;
	boxs->flag = 0;

	list_init(&boxs->boxs);
	boxs->boxsNum = 0;

	list_init(&boxs->caches);
	boxs->cachesNum = 0;
	boxs->cachesMax = KMBOX_CACHE_MAX;
	boxs->cachesMin = KMBOX_CACHE_MIN;
	
	boxs->extend = NULL;
	
	return;
}


KvmBox* new_kvmBox(){
	KvmBox *box = (KvmBox*)mobj_new(sizeof(KvmBox));
	if(NULL==box){
		return NULL;
	}
	kvmBox_init(box);
	return box;
}


bool_t del_kvmBox(KvmBox *box){
	if(NULL==box){
		return FALSE;
	}
	return mobj_delete(box,sizeof(KvmBox));
}


KvmBoxs* new_kvmBoxs(){
	KvmBoxs* boxs = (KvmBoxs*)mobj_new(sizeof(KvmBoxs));
	if(NULL==boxs){
		return NULL;
	}
	kvmBoxs_init(boxs);
	return boxs;
}


bool_t del_kvmBoxs(KvmBoxs *boxs){
	if(NULL==boxs){
		return FALSE;
	}
	return mobj_delete(boxs,sizeof(KvmBoxs));
}
