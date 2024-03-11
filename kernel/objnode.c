/**
 * 
 */
#include "globaltype.h"
#include "globalctrl.h"


/**
 * 默认初始化objnode_t
 */
void objnode_t_init(objnode_t* node){
	spinlock_init(&node->lock);
	list_t_init(&node->hook);
	sem_t_init(&node->sem);
	node->flags = 0;
	node->status = 0;
	node->opcode = 0;
	node->objtype = 0;
	node->objadr = NULL;
	node->acsflags = 0;
	node->acsstus = 0;
	node->curloc = 0;
	node->len = 0;
	node->ioctrl = 0;
	node->buf = NULL;
	node->bufcurloc = 0;
	node->bufsz = 0;
	node->count = 0;
	node->safedsc = NULL;
	node->fname = NULL;
	node->fnode = NULL;
	node->expt = NULL;
	
	return;
}


objnode_t* objnode_new(){
	objnode_t* node = (objnode_t*)mempool_new(sizeof(objnode_t));
	if(NULL==node){
		return NULL;
	}
	//初始化objnode_t
	objnode_t_init(node);
	return node;
}


bool_t objnode_delete(objnode_t* node){
	if(mempool_delete((addr_t)node,sizeof(objnode_t))==FALSE){
		printk("delete_objnode error\n");die(0);
		return FALSE;
	}

	return TRUE;
}


drvstus_t device_objnode_param(void *objnode,buf_t* buf,u64_t* curloc,u64_t* len,u64_t* ioctrl,u64_t* bufcurloc,size_t* bufsz){
	objnode_t* obj = (objnode_t*)objnode;
	//参数检查
	if(NULL==obj){
		return DFCERRSTUS;
	}

	//设备读写数据的缓冲区
	if(NULL!=buf){
		*buf = obj->buf;
	}

	//设备读写数据的当前位置
	if(NULL!=curloc){
		*curloc = obj->curloc;
	}

	//设备读写数据的长度
	if(NULL!=len){
		*len = obj->len;
	}

	//IO控制码
	if(NULL!=ioctrl){
		*ioctrl = obj->ioctrl;
	}

	//对应于读写数据缓冲区的当前位置
	if(NULL!=bufcurloc){
		*bufcurloc = obj->bufcurloc;
	}

	//对应于读写数据的缓冲区的大小
	if(NULL!=bufsz){
		*bufsz = obj->bufsz;
	}

	return DFCOKSTUS;
}




