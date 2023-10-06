/**
 * 内存页初始化
 */
#include "globaltype.h"
#include "globalctrl.h"


/**
 * initializing 'PageAddr' simply
 * addr: 
 */
static void init_pageaddr(PageAddr* addr){
	memset((addr_t)addr,0,sizeof(PageAddr));
	return ;
}

/**
 * initializing the page descriptor simply
 * page: struct pointer of MemPage
 */ 
static void init_mempage(MemPage* page){
	//initilizing list and lock
	list_init(&page->hook);
	spinlock_init(&page->lock);
	//page flags:set initial value
	page->flags.olkty = PAGEFLAGS_OLKTY_INIT;
	page->flags.lstty = PAGEFLAGS_LSTTY_LIST;
	page->flags.mocty = PAGEFLAGS_MOCTY_FREE;
	page->flags.marty = PAGEFLAGS_MARTY_INIT;
	page->flags.count = PAGEFLAGS_UINDX_INIT;
	//page physical address
	init_pageaddr(&page->addr);
	//page link
	page->next = NULL;
	return;	
}

/**
 * assigning specific value for MemPage object 
 * page: page descriptor
 * phyadr: page's physical address
 * 
 */
static void set_mempage(MemPage* page,addr_t phyadr){
	//对页描述符简单初始化
	init_mempage(page);
	//取出phyadr数据地址，使用PageAddr结构体来解释该数据
	PageAddr* padr = (PageAddr*)(&phyadr);
	//把页号写入页描述符
	page->addr.value = padr->value;
	
	return;
}

/**
 * counting how many pages are needs
 * return:size of array, that is, the number of descriptors
 */
static size_t count_pages(){
	Machine *mach = &machine;
	MemView* views = (MemView*)(mach->e820s_addr);
	size_t index = 0;
	
	for(uint_t i=0; i<mach->e820s_num; i++){
		if(RAM_USABLE==views[i].type){
			for(addr_t adr=views[i].addr; adr+0xfff<=views[i].end; adr+=0x1000){
				index++;
			}
		}
	}

	return index;
}

/**
 * principle of memory partition：directly discard the page less than 4KB in size to avoid incomplete pages
 */
void mempage_init(){
	Machine *mach = &machine;
	MemView *views = (MemView*)(mach->e820s_addr);

	//counting the length of 'pages' array
	size_t len = count_pages();
	MemPage *pages = (MemPage*)MAP_RV(mach->next_addr);		//分配内存

	//traversing array of e820s and establishing table of pages for all usable memory   
	for(uint_t i=0,index=0;i<mach->e820s_num;i++){
		//judging whether it's usable memory or not
		if(RAM_USABLE==views[i].type){
			//starting from the beginning address of memmory segment,process each page with 4KB in size sequentially
			for(addr_t adr=views[i].addr; adr+0xfff<=views[i].end; adr+=0x1000){
				//fill the information collected above in 'pages[index]' 
				set_mempage(&pages[index],adr);
				index++;
			}
		}
	}

	//filling in the struct of machine information
	mach->pages_addr = (u64_t)pages;
	mach->pages_num = len;
	mach->pages_size = len*sizeof(MemPage);
	//calculating the next address aligned by 4K
	mach->next_addr = ALIGN_4K(mach->next_addr+mach->pages_size);
	
	return;
}


/**
 * 标记占用的内存区域为“已分配”
 * start: 数据首字节地址
 * size: 数据占用内存大小(B)
 * 数据标记原则：
 * 	start和end所在页也要计算在内
 */
static void mark_occupy(addr_t start,size_t size){
	MemPage* pages = (MemPage*)machine.pages_addr;
	size_t len = machine.pages_num;
	addr_t end = start+size-1;		//计算数据尾字节地址
	
	for(size_t i=0;i<len;i++){
		//确定start所在页的页号i
		if( pages[i].addr.value == (start>>12) ){	//pages[i]的页号=start地址所在页号
			//从页号i开始标记连续的页(包括end所在页)
			for(;(pages[i].addr.value<<12) <= end;i++){	//pages[i]的页地址<end地址
				//已分配给内核;分配计数加一;
				pages[i].flags.mocty = PAGEFLAGS_MOCTY_KRNL;	//分配类型：内核(初始化阶段占用的内存是分配给了内核)
				pages[i].flags.count++;
				pages[i].addr.allocate = PAGEADDR_ALLOC;	//已分配
			}
			return;
		}
	}
}


/**
 * 标记内核占用的所有内存：内核本身，内核持有的各种数据结构
 * 注意：x86的BIOS中断表从内存地址0x0开始,每个表项2B，共256项，用于处理系统级别的中断
 */
void mempage_occupy(){
	Machine* mach = &machine;
	
	//BIOS中断表占用内存(这里并不是bios.bin，bios.bin也是借助BIOS中断(表)来实现功能的)
	mark_occupy(0,0x1000);
	
	//内核栈占用内存（向下4KB对齐：栈向低地址拓展且栈底地址才是栈地址）
	mark_occupy(mach->stack_addr&(~0xfffUL),mach->stack_size);

	//内核映像文件占用内存
	mark_occupy(mach->image_addr,mach->image_size);

	//拓展后的kernel.bin占用内存
	mark_occupy(mach->kernel_addr,mach->next_addr-mach->kernel_addr);

	/**
	 * kernel.bin执行过程中动态分配的内存都连续存放在了kernel.bin后面。
	 * 由于linker脚本的设置，编译时变量编织在了kernel.bin中)，
	 * 所以我可以在kernel.bin文件后面继续分配动态内存，并把他们看作kernel.bin文件的一部分
	 * 而无需考虑kernel.bin的增长会覆盖原始kernel.bin中变量数据
	 * (Memorys)e820s数组
	 * font字体
	 * 内存页表
	 * MMU页表
	 * 因此在链接文件中，需要让内核的各个段连续存放，避免被覆盖
	 */
	return;
}



//=================================================================================================================
INLINE void test_mark();

void mempage_test_main(){



	die(0);
	return;
}

/**
 * 测试标记情况
 */
INLINE void test_mark(){
	MemPage* pages = (MemPage*)machine.pages_addr;
	size_t len = machine.pages_num;
	printk("0x%lx\t",pages[0].addr.value<<12);
	printk("0x%lx\n",pages[len-1].addr.value<<12);
	for(size_t i=0;i<len;i++){
		if(pages[i].flags.mocty==PAGEFLAGS_MOCTY_KRNL){
			printk("occupy:0x%lx\t",pages[i].addr.value<<12);	//打印出占用内存区起始页
			while(pages[i].flags.mocty==PAGEFLAGS_MOCTY_KRNL) i++;
			printk("0x%lx\n",pages[i].addr.value<<12);	//打印出占用内存区末位页的下一页
		}
	}

	printk("next_addr:0x%lx\n",machine.next_addr);
	printk("image_size:0x%lx\n",machine.image_size);	
	
	return;
}

/**
 * 
 */
void mempage_display(){
	Machine* mach = &machine;
	printk("pages address:0x%lx\n",mach->pages_addr);
	printk("pages number:0x%lx\n",mach->pages_num);
	printk("pages size:0x%lx\n",mach->pages_size);
	
	test_mark();
	return;
}
