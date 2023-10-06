/**
 * 二级引导器主模块
 */
#include"global.h"

static void loader_init();

/**
 * entry of loader
 */
void loader_entry(){
	init_cursor();			//模块切换重新初始化文本模式
	clear_screen(0x0f00);
	printk("=================================loader start===============================\n");

	loader_init();
	
	printk("=================================loader end.================================\n");
	pause("");
	return;
}

/**
 * initializing loader
 */
static void loader_init(){
	
	//创建机器信息结构体并初始化
	Machine *machine = (Machine*)MACHINE_ADR;	//机器信息结构体指针
	//收集机器信息
	machine_init(machine);
	//输出机器信息
	machine_display(*machine);

	return ;
}

void pause(){
	real_entry(2,0,0);	//通过实模式调用拷贝字符串
	return;
}