/**
 * initializations for platform
 * API realization
 */
#include "globaltype.h"
#include "globalctrl.h"

void platform_init(){
	machine_t* mach = &machine;
	//clearing memory of Machine struct
	memset(mach,0,sizeof(machine_t));	//mach is the address allocated by compiler，existing in kernel.bin
	//copying 'mach'
	memcpy(mach,(void*)MACHINE_ADR,sizeof(machine_t));	
	
	return;
}

void platform_display(){
	printk("stack addr:0x%lx\n",machine.stack_addr);
	printk("stack size:0x%lx\n",machine.stack_size);
	printk("mmu addr:0x%lx\n",machine.mmu_addr);
	printk("mmu size:0x%lx\n",machine.mmu_size);		
	printk("cpu mode:0x%lx\n",machine.cpu_mode);
	printk("mem size:0x%lx\n",machine.mem_size);
	printk("e820s addr:0x%lx\n",machine.e820s_addr);
	printk("e820s num:0x%lx\n",machine.e820s_num);	
	printk("e820s size:0x%lx\n",machine.e820s_size);
	printk("image addr:0x%lx\n",machine.image_addr);
	printk("image size:0x%lx\n",machine.image_size);	
	printk("font addr:0x%lx\n",machine.font_addr);
	printk("font size:0x%lx\n",machine.font_size);
	printk("kernel addr:0x%lx\n",machine.kernel_addr);
	printk("kernel size:0x%lx\n",machine.kernel_size);
	printk("viraddr size:0x%lx\n",machine.viraddr_size);
	printk("pages addr:0x%lx\n",machine.pages_addr);
	printk("pages num:0x%lx\n",machine.pages_num);	
	printk("pages size:0x%lx\n",machine.pages_size);	
	printk("areas addr:0x%lx\n",machine.areas_addr);
	printk("areas num:0x%lx\n",machine.areas_num);	
	printk("areas size:0x%lx\n",machine.areas_size);		
	printk("next addr:0x%lx\n",machine.next_addr);	
	printk("checksum:0x%lx\n",machine.checksum);

	return;
}
