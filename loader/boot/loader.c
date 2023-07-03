/**
 * 二级引导器主模块
 */
#include"global.h"


/**
 * 二级引导器入口函数
 */
void loader_entry(){
	init_cursor();			//模块切换重新初始化文本模式
	clear_screen(0x0f00);

	//加载位示图
	allocate_init(BITMAP_ADR,0x100000,0x8000000);
	
	printk("=================================loader start===============================\n");
	Machine *machine = (Machine*)MACHINE_ADR;	//机器信息结构体
	init_machine(machine);
	display_machine(machine);
	printk("=================================loader end.================================\n");
	read_string("");
	return;
}

void read_string(const char_t* str){
	real_entry(2,0,0);	//通过实模式调用拷贝字符串
	return;
}