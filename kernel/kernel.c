/**
 * 内核初始化
 */
#include "globalctrl.h"
#include "globaltype.h"

void kernel_entry(){
	init_cursor();
	clear_screen(0x0f00);
	printk("welcome to the wind os kernel...\n");

	//创建中断句柄
	IntHandle ihandle0;
	IntHandle ihandle1;
	init_intHandle(&ihandle0,1,&intFaults[0],(void*)0x1111,callback_divide0);
	init_intHandle(&ihandle1,0,&intFaults[0],(void*)0x2222,callback_divide1);
	add_ihandle(&intFaults[0],&ihandle0);
	add_ihandle(&intFaults[0],&ihandle1);
	
	for(sint_t i=5,j;i>=0;i--){
		j=10/i;
		printk("%d\n",j);	//发生除零错误
		die(0x200);
	}
	
	die(0);
	return ;
}
