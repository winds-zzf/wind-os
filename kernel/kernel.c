/**
 * 内核初始化
 */
#include "globalctrl.h"
#include "globaltype.h"



/**
 * 进入内核之后直到关机，之前就不再退出了，至多发生一些进程切换
 */
void kernel_entry(){
	init_cursor();
	clear_screen(0x0f00);
	printk("welcome to the wind os kernel...\n");
	
	/**
	 * 初始化内核各个模块
	 */
	
	//1.初始化内核功能层内存管理模块
	init_krlmm();
	//2.初始化设备
	device_init();
	//3.初始化调度器
	schedule_init();
	//初始化空转进程，开启进程调度
	idle_init();
	//5.初始化设备
	
	//5.初始化磁盘
	
	//6.初始化网络
	
	//7.初始化接口

	
	die(0);
	return;
}






/**
 * 1.内核代码使用虚拟地址，但是内核有时需要用到物理地址，比如设置页表项等，
 * 线性映射区使得内核能够通过加减一个固定值的方式，方便的完成虚拟地址和物理地址的转换
 * 
 * 2.内核工作在长模式下使用虚拟地址，并通过MMU页表实现虚拟地址到物理地址的转换，
 * 至于怎么转换，虚拟地址对应的物理地址是否是线性的，其实都不重要，可以是任意不冲突的
 * 
 * 3.MMU本身就是做虚拟地址和物理地址的映射的，因此MMU不能建立在虚拟地址之上，
 * 但是MMU页表是放在内核只中的，
 * 
 * 5.应用进程也是工作在虚拟地址空间，通过MMU进行虚拟地址空间到物理地址空间的映射，
 * 如果映射的物理页不在内存，将触发缺页异常中断，虚拟内存是不会发生缺失的，发生缺失的是虚拟内存对应的物理页
 * 
 * 6.内核本身会发生缺页异常吗？内核本身占用的内存不会参与内存分配，属于不可用状态
 * 
 * 7.
 * 
 * 
 * 
 * 
 */