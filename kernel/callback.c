/**
 * 回调函数测试
 */
#include "globaltype.h"
#include "globalctrl.h"

/**
 * 
 */

drvstus_t callback_divide0(uint_t ifault_num,void* device,void* sframe){
	printk("[0]:you have run into an zero-divide error!!!\n");
	printk("ifault_num:0x%x\n",ifault_num);
	printk("device:0x%lx\n",(addr_t)device);
	printk("sframe:0x%lx\n",(addr_t)sframe);
	return 1;
}


drvstus_t callback_divide1(uint_t ifault_num,void* device,void* sframe){
	printk("[1]:you have run into an zero-divide error!!!\n");
	printk("ifault_num:0x%x\n",ifault_num);
	printk("device:0x%lx\n",(addr_t)device);
	printk("sframe:0x%lx\n",(addr_t)sframe);
	return 1;
}
