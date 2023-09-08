/**
 * 内存管理模块
 */

#include "globaltype.h"
#include "globalctrl.h"


void memmgr_init(){
	//初始化页描述符信息
	mempage_init();
	printk("finishing initializing memory pages\n");
	mempage_display();

	/*
	//初始化页描述符表
	memtable_init();
	printk("finishing initializing memory tables\n");	
	memtable_display();
	*/
	
	//初始化内存区
	memarea_init();
	printk("finishing initializing memory areas\n");	
	memarea_display();

	return;
}


void memmgr_display(){
	
	return;
}
