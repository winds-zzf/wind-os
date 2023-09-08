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
	platform_display();

	die(0);
	//进入内核层
	kernel_entry();
	
	printk("===================================hal end.==================================\n");
	die(0x3F0);
    	return;
}

static void hal_init(){
	//初始化简单内存管理（由于平坦映射，这里的虚拟地址值就等于物理地址值）
	allocate_init(MAP_RV(0x30000),MAP_RV(0x100000),0x8000000);
	
	//1.初始化平台
	platform_init(&machine);		//直接使用已经定义的全局变量
	platform_display(&machine);	//显示	
	printk("finish initializing platform...\n");

	//2.初始化内存
	memory_init(&machine);
	memory_display(&machine);
	printk("finish initializing memory...\n");
	
	//3.初始化中断
	interrupt_init(&machine);
	interrupt_display(&machine);
	printk("finish initializing interrupt...\n");
	
	return;
}
