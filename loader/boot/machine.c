/**
 * 机器信息模块
 * API实现
 */
#include"global.h"
#include"machine_t.h"

static void stack_init(Machine* mach);
static void stack_display(Machine mach);

static void kernel_init(Machine* mach);
static void kernel_display(Machine mach);

static void font_init(Machine* mach);
static void font_display(Machine mach);

static void logo_init(Machine* mach);
static void logo_display(Machine mach);

/**
 * 收集机器信息
 */
void machine_init(Machine *mach){
	memset((addr_t)mach,0,sizeof(Machine));		//清空Machine变量内存
	printk("finishing initializing Machine...\n");
	pause();

	//记录内核映像文件
	mach->image_addr = IMAGE_ADR;
	mach->image_size = get_image_size(IMAGE_ADR);
	
	//初始化CPU
	cpu_init(mach);
	printk("finishing initializing cpu...\n");
	cpu_display(*mach);	
	pause();
	
	//初始化内存视图
	e820_init(mach);
	printk("finishing initializing e820...\n");
	e820_display(*mach);
	pause();
	
	//初始化acpi
	acpi_init(mach);
	printk("finishing initializing acpi...\n");
	acpi_display(*mach);
	pause();

	//初始化内核栈
	stack_init(mach);
	printk("finishing initializing stack...\n");
	stack_display(*mach);
	pause();

	//放置内核文件
	kernel_init(mach);
	printk("finishing initializing kernel...\n");
	kernel_display(*mach);
	pause();
	
	//放置字库文件
	font_init(mach);
	printk("finishing initializing font...\n");
	font_display(*mach);
	pause();

	//放置logo文件
	logo_init(mach);
	printk("finishing initializing logo...\n");
	logo_display(*mach);
	pause();
	
	//初始化mmu页表
	mmu_init(mach);
	printk("finishing initializing mmu...\n");
	mmu_display(*mach);
	
	pause();
	//初始化图形模式
	graph_init(mach);
	printk("finishing initializing graph...\n");
	graph_display(*mach);
	pause();
		
	//初始化磁盘
	disk_init(mach);
	printk("finishing initializing disk...\n");
	disk_display(*mach);	
	pause();

	return;
}

void machine_display(Machine mach){
	printk("stack addr:0x%lx\n",mach.stack_addr);
	printk("stack size:0x%lx\n",mach.stack_size);
	printk("mmu addr:0x%lx\n",mach.mmu_addr);
	printk("mmu size:0x%lx\n",mach.mmu_size);		
	printk("cpu mode:0x%lx\n",mach.cpu_mode);
	printk("mem size:0x%lx\n",mach.mem_size);
	printk("e820s addr:0x%lx\n",mach.e820s_addr);
	printk("e820s num:0x%lx\n",mach.e820s_num);	
	printk("e820s size:0x%lx\n",mach.e820s_size);
	printk("image addr:0x%lx\n",mach.image_addr);
	printk("image size:0x%lx\n",mach.image_size);	
	printk("font addr:0x%lx\n",mach.font_addr);
	printk("font size:0x%lx\n",mach.font_size);
	printk("kernel addr:0x%lx\n",mach.kernel_addr);
	printk("kernel size:0x%lx\n",mach.kernel_size);
	printk("viraddr size:0x%lx\n",mach.viraddr_size);	
	printk("checksum:0x%lx\n",mach.checksum);	
	return;
}

/**
 * 初始化内核栈
 * 编译器在编译过程会生成堆栈的操作，但是并不会在目标模块中包含栈空间，
 * 应用程序的栈是由操作系统分配的，内核程序的栈就要我们加载内核时手动分配了。
 */
static void stack_init(Machine* mach){
	//填写机器信息
	mach->stack_addr = (u64_t)STACK_ADR;
	mach->stack_size = (u64_t)STACK_SIZE;	
	
	return;
}

/**
 * 
 */
static void stack_display(Machine mach){
	printk("=================================stack info===================================\n");
	printk("stack addr:0x%lx\n",mach.stack_addr);
	printk("stack size:0x%lx\n",mach.stack_size);
	printk("=================================stack end.===================================\n");
	
	return;
}


/**
 * 加载内核文件
 * 将内核文件放置到指定位置
 */
static void kernel_init(Machine* mach){
	addr_t addr = NULL;
	size_t size = 0;
	
	mach->next_addr = KERNEL_ADR;	//crucial:set the kernel.bin's address as next_addr's initial address
	
	//获取内核文件信息
	size = get_image_file(IMAGE_ADR,KERNEL_NAME,&addr); 
	if(0==size || NULL==addr){
		printk("missing file:<%s>\n",KERNEL_NAME);
		return;
	}

	//复制到指定位置
	memcpy(addr,size,KERNEL_ADR);	//将内核文件复制到KERNEL_ADR位置

	//填写机器信息
	mach->kernel_addr = KERNEL_ADR;
	mach->kernel_size = (u64_t)size;

	//初始化next_addr:使其紧挨着kernel存放
	mach->next_addr = ALIGN_4K(KERNEL_ADR+mach->kernel_size);
	
	return;
}

/**
 * 
 */
static void kernel_display(Machine mach){
	printk("================================kernel info===================================\n");
	printk("kernel addr:0x%lx\n",mach.kernel_addr);
	printk("kernel size:0x%lx\n",mach.kernel_size);	
	printk("================================kernel end.===================================\n");
	
	return;
}

/**
 * 加载字体文件
 * 将字体文件放置到指定位置
 */
static void font_init(Machine* mach){
	addr_t addr = NULL;
	size_t size = 0;
	
	//获取字体文件
	size = get_image_file(IMAGE_ADR,FONT_NAME,&addr);
	if(0==size||NULL==addr){
		printk("missing file:<%s>\n",FONT_NAME);	
		return ;
	}
	
	//复制到指定位置
	addr_t dst = (addr_t)mach->next_addr;	//内存分配
	memcpy(addr,size,dst);
	
	//填写机器信息结构体
	mach->font_addr = (u64_t)dst;
	mach->font_size = (u64_t)size;

	//计算下一个4K对齐地址
	mach->next_addr = ALIGN_4K(mach->next_addr+mach->font_size);
	
	return;
}


/**
 * 输出字体信息
 */
static void font_display(Machine mach){
	printk("=================================font info====================================\n");
	printk("font addr:0x%lx\n",mach.font_addr);
	printk("font size:0x%lx\n",mach.font_size);	
	printk("=================================font end.====================================\n");
	
	return;
}

/**
 * 加载字体文件
 * 将字体文件放置到指定位置
 */
static void logo_init(Machine* mach){
	addr_t addr = NULL;
	size_t size = 0;
	
	//获取字体文件
	size = get_image_file(IMAGE_ADR,LOGO_NAME,&addr);
	if(0==size||NULL==addr){
		printk("missing file:<%s>\n",LOGO_NAME);	
		return ;
	}
	
	//复制到指定位置
	addr_t dst = (addr_t)mach->next_addr;	//内存分配
	memcpy(addr,size,dst);
	
	//填写机器信息结构体

	//计算下一个4K对齐地址
	mach->next_addr = ALIGN_4K(mach->next_addr+size);
	
	return;
}


/**
 * 显示logo信息
 * mach: 机器信息结构体
 */
static void logo_display(Machine mach){
	printk("=================================logo info====================================\n");
	printk("logo addr:0x%lx\n",0);
	printk("logo size:0x%lx\n",0);	
	printk("=================================logo end.====================================\n");
	
	return;
}
