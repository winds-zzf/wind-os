/**
 * 平台初始化
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

void platform_init(){
	Machine* mach = &machine;
	//清空机器信息结构体对应内存
	memset((addr_t)mach,0,sizeof(Machine));	//mach是编译器分配的地址，链接器中已经指定为虚拟地址了
	//复制机器信息结构体（因为有平坦模型的存在，使用物理地址也是没问题的）
	memcpy(MACHINE_ADR,sizeof(Machine),(addr_t)mach);
	//释放原先手动分配的Machine结构体的内存
	release(MAP_RV(MACHINE_ADR),sizeof(Machine));
	
	return;
}

void platform_display(){
	printk("stack addr:0x%lx\n",machine.stack_addr);
	printk("stack size:0x%lx\n",machine.stack_size);
	printk("page addr:0x%lx\n",machine.page_addr);
	printk("page size:0x%lx\n",machine.page_size);		
	printk("cpu mode:0x%lx\n",machine.cpu_mode);
	printk("mem size:0x%lx\n",machine.mem_size);
	printk("e820s addr:0x%lx\n",machine.e820s_addr);
	printk("e820s num:0x%lx\n",machine.e820s_num);	
	printk("e820s size:0x%lx\n",machine.e820s_size);
	printk("font addr:0x%lx\n",machine.font_addr);
	printk("font size:0x%lx\n",machine.font_size);
	printk("kernel addr:0x%lx\n",machine.kernel_addr);
	printk("kernel size:0x%lx\n",machine.kernel_size);
	printk("viraddr size:0x%lx\n",machine.viraddr_size);
	printk("pages addr:0x%lx\n",machine.pages_addr);
	printk("pages num:0x%lx\n",machine.pages_num);	
	printk("pages size:0x%lx\n",machine.pages_size);	
	printk("bitmap addr:0x%lx\n",machine.bitmap_addr);
	printk("bitmap size:0x%lx\n",machine.bitmap_size);	
	printk("checksum:0x%lx\n",machine.checksum);
	return;
}
