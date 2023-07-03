/**
 * 机器信息模块
 * API实现
 */
#include"global.h"
#include"machine_t.h"

static void stack_init(Machine* mach);
static void stack_display(Machine* mach);

static void kernel_init(Machine* mach);
static void kernel_display(Machine* mach);

static void font_init(Machine* mach);
static void font_display(Machine* mach);

static void logo_init(Machine* mach);
static void logo_display(Machine* mach);

/**
 * 收集机器信息
 */
void init_machine(Machine *mach){
	memset(MACHINE_ADR,0,sizeof(Machine));		//清空Machine变量内存
	reserve(MACHINE_ADR,sizeof(Machine));		//占用指定位置的内存
	read_string("");
	
	//初始化CPU
	cpu_init(mach);
	printk("finishing initializing cpu...\n");
	cpu_display(mach);	
	read_string("");
	
	//初始化内存视图
	e820_init(mach);
	printk("finishing initializing e820...\n");
	e820_display(mach);
	read_string("");
	
	//初始化acpi
	acpi_init(mach);
	printk("finishing initializing acpi...\n");
	acpi_display(mach);
	read_string("");

	//初始化内核栈
	stack_init(mach);
	printk("finishing initializing stack...\n");
	stack_display(mach);
	read_string("");

	//放置内核文件
	kernel_init(mach);
	printk("finishing initializing kernel...\n");
	kernel_display(mach);
	read_string("");
	
	//放置字库文件
	font_init(mach);
	printk("finishing initializing font...\n");
	font_display(mach);
	read_string("");

	//放置logo文件
	logo_init(mach);
	printk("finishing initializing logo...\n");
	logo_display(mach);
	read_string("");
	
	//初始化mmu页表
	mmu_init(mach);
	printk("finishing initializing mmu...\n");
	mmu_display(mach);
	read_string("");
	
	//初始化图形模式
	graph_init(mach);
	printk("finishing initializing graph...\n");
	graph_display(mach);
	read_string("");
		
	//初始化磁盘
	disk_init(mach);
	printk("finishing initializing disk...\n");
	disk_display(mach);	
	read_string("");
	
	//将bitmap移动到合适位置
	//mach->bitmap_addr = allocate(0x1000);
	//mach->bitmap_size = 0x1000;
	//memcpy(BITMAP_ADR,(size_t)mach->bitmap_size,(addr_t)mach->bitmap_addr);
	return;
}

void display_machine(Machine *mach){
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
	printk("checksum:0x%x\n",mach->checksum);	
	printk("bitmap addr:0x%x\n",mach->bitmap_addr);
	printk("bitmap size:%d\n",mach->bitmap_size);	
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
static void stack_display(Machine* mach){
	
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
	mach->kernel_addr = (u64_t)size;
	return;
}

/**
 * 
 */
static void kernel_display(Machine* mach){
	printk("kernel addr:0x%x\n",mach->kernel_addr);
	printk("kernel size:%d\n",mach->kernel_size);	
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
	mach->font_addr = dst;
	mach->font_size = size;
	return;
}


/**
 * 
 */
static void font_display(Machine* mach){
	printk("font addr:0x%x\n",mach->font_addr);
	printk("font size:%d\n",mach->font_size);	
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
 * 
 */
static void logo_display(Machine* mach){
	printk("logo addr:\n");
	printk("logo size:\n");	
	return;
}