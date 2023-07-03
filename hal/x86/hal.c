/**
 * 内核入口开始文件
 */
#include"globaltype.h"
#include"globalctrl.h"

static void init_hal();

/**
 * 内核初始化总控函数
 */
void hal_entry(){
	init_cursor();
	clear_screen(0x0f00);
	
	printk("===================================hal start==================================\n");
	init_hal();
	printk("===================================hal end.==================================\n");	
	
	die(0);
    	return;
}

static void init_hal(){
	//初始化简单内存管理，由于平坦映射，这里的虚拟地址值就等于物理地址值
	allocate_init(KERNEL_VIRADR_START+0x30000,KERNEL_VIRADR_START+0x100000,0x8000000);
	
	//初始化平台
	platform_init(&machine);	//直接使用已经定义的全局变量
	platform_display(&machine);	//显示	
	
	die(0);
	//初始化内存
	memory_init(&machine);
	memory_display(&machine);
	
	//初始化中断


	
	return;
}
