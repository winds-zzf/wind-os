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

static void bitmap_init(Machine* mach);
static void bitmap_display(Machine mach);

/**
 * 收集机器信息
 */
void init_machine(Machine *mach){
	reserve(MACHINE_ADR,sizeof(Machine));		//占用指定位置的内存
	memset(MACHINE_ADR,0,sizeof(Machine));		//清空Machine变量内存
	printk("finishing initializing Machine...\n");
	pause();

	//初始化简单内存分配
	bitmap_init(mach);
	printk("finishing initializing bitmap...\n");
	bitmap_display(*mach);
	pause();
	
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

void display_machine(Machine mach){
	printk("stack addr:0x%lx\n",mach.stack_addr);
	printk("stack size:0x%lx\n",mach.stack_size);
	printk("page addr:0x%lx\n",mach.page_addr);
	printk("page size:0x%lx\n",mach.page_size);		
	printk("cpu mode:0x%lx\n",mach.cpu_mode);
	printk("mem size:0x%lx\n",mach.mem_size);
	printk("e820 addr:0x%lx\n",mach.e820s_addr);
	printk("e820 num:0x%lx\n",mach.e820s_num);	
	printk("e820 size:0x%lx\n",mach.e820s_size);
	printk("font addr:0x%lx\n",mach.font_addr);
	printk("font size:0x%lx\n",mach.font_size);
	printk("kernel addr:0x%lx\n",mach.kernel_addr);
	printk("kernel size:0x%lx\n",mach.kernel_size);
	printk("viraddr size:0x%lx\n",mach.viraddr_size);	
	printk("bitmap addr:0x%lx\n",mach.bitmap_addr);
	printk("bitmap size:0x%lx\n",mach.bitmap_size);	
	printk("checksum:0x%lx\n",mach.checksum);	
	return;
}

/**
 * 初始化内核栈
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
	//获取内核文件信息
	size = get_image_file(IMAGE_ADR,KERNEL_NAME,&addr); 
	if(0==size || NULL==addr){
		printk("missing file:<%s>\n",KERNEL_NAME);
		return;
	}

	//复制到指定位置
	reserve(KERNEL_ADR,size);	//将对应内存标记为已占用
	memcpy(addr,size,KERNEL_ADR);	//将内核文件复制到KERNEL_ADR位置
	
	//填写机器信息
	mach->kernel_addr = (u64_t)KERNEL_ADR;
	mach->kernel_size = (u64_t)size;
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
	addr_t dst = allocate(size);	//内存分配
	memcpy(addr,size,dst);
	
	//填写机器信息结构体
	mach->font_addr = (u64_t)dst;
	mach->font_size = (u64_t)size;
	return;
}


/**
 * 
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
	addr_t dst = allocate(size);	//内存分配
	memcpy(addr,size,dst);
	
	//填写机器信息结构体

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

static void bitmap_init(Machine* mach){
	mach->bitmap_addr = (u64_t)allocate(0x1000);
	mach->bitmap_size = 0x1000;
	memcpy(BITMAP_ADR,(size_t)mach->bitmap_size,(addr_t)mach->bitmap_addr);
	return;
}
static void bitmap_display(Machine mach){
	printk("================================bitmap info===================================\n");
	printk("bitmap addr:0x%lx\n",mach.bitmap_addr);
	printk("bitmap size:0x%lx\n",mach.bitmap_size);	
	printk("================================bitmap end.===================================\n");
	return;
}
