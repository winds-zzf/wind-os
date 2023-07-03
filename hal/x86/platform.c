/**
 * 平台初始化
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

void platform_init(Machine* mach){
	//清空机器信息结构体对应内存
	memset((addr_t)mach,0,sizeof(Machine));	//mach是编译器分配的地址，链接器中已经指定为虚拟地址了
	
	//复制机器信息结构体（因为有平坦模型的存在，使用物理地址也是没问题的）
	memcpy(MACHINE_ADR,sizeof(Machine),(addr_t)mach);
	release(MACHINE_ADR,sizeof(Machine));	//释放手动分配的Machine结构体的内存

	die(0);
	return;
}

void platform_display(Machine* mach){
	printk("stack addr:0x%x\n",mach->stack_addr);
	printk("stack size:%d\n",mach->stack_size);
	printk("page addr:0x%x\n",mach->page_addr);
	printk("page size:%d\n",mach->page_size);		
	printk("cpu mode:%d\n",mach->cpu_mode);
	printk("mem size:%d\n",mach->mem_size);
	printk("e820 addr:0x%x\n",mach->e820_addr);
	printk("e820 num:%d\n",mach->e820_num);	
	printk("e820 size:%d\n",mach->e820_size);
	printk("font addr:0x%x\n",mach->font_addr);
	printk("font size:%d\n",mach->font_size);
	printk("viraddr size:%d\n",mach->viraddr_size);
	printk("bitmap addr:0x%x\n",mach->bitmap_addr);
	printk("bitmap size:%d\n",mach->bitmap_size);	
	printk("checksum:0x%x\n",mach->checksum);
	return;
}
