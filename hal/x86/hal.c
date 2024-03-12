/**
 * 内核入口开始文件
 */
#include"globaltype.h"
#include"globalctrl.h"

static void hal_init();

/**
 * 内核初始化总控函数
 * 内核hal层初始化用于将前期收集的信息正规化（变成程序中的结构体，变量等）
 * hal层已经是64位模式了。
 */
void hal_entry(){
	init_cursor();
	clear_screen(0x0f00);
	
	printk("===================================hal start==================================\n");	
	//初始化内核抽象层
	hal_init();
	
	printk("===================================hal end.==================================\n");
    	
    	//进入内核层
	kernel_entry();
    	return;
}

static void hal_init(){	
	
	//1.initializing platform
	platform_init(&machine);		//直接使用已经定义的全局变量
	platform_display(&machine);	//显示	
	printk("finish initializing platform...\n");
	
	//2.initializing interrupt
	interrupt_init(&machine);
	interrupt_display(&machine);
	printk("finish initializing interrupt...\n");

	//3.initializing memmgr
	memmgr_init(&machine);
	memmgr_display(&machine);
	printk("finish initializing memory...\n");
	
	return;
}
