/**
 * 内存页初始化
 */
#include "globaltype.h"
#include "globalctrl.h"


/**
 * 简单初始化PageAddr
 * addr: 
 */
static void init_pageaddr(PageAddr* addr){
	return ;
}

/**
 * 简单初始化页描述符
 * page: page结构体指针
 */ 
static void init_page(Page* page){
	//
	#pragma GCC diagnostic ignored"-Waddress-of-packed-member"
	list_init(&page->list);
	#pragma GCC diagnostic ignored"-Waddress-of-packed-member"
	knl_spinlock_init(&page->lock);
	//page flags
	page->flags.olkty = 0;
	page->flags.lstty = 0;
	page->flags.mocty = 0;
	page->flags.marty = 0;
	page->flags.uindx = 0;
	//page address
	init_pageaddr(&page->addr);
	page->next = NULL;
	return;	
}

/**
 * 给page结构体设定值
 * page: 页描述符
 * phyadr: 页物理地址值
 * 
 */
static void set_page(Page* page,addr_t phyadr){
	//对页描述符简单初始化
	init_page(page);
	//将phyadr的地址传给PageAddr结构体指针，以便以访问结构体成员的方式处理phyadr的值
	PageAddr* padr = (PageAddr*)(&phyadr);
	//把页号写入页描述符
	page->addr.value = padr->value;
	
	return;
}

/**
 * 统计需要多少页面，并创建相应的page数组
 * 
 * return:数组大小，也即页描述符数量
 */
static size_t create_pages(Page **pages){
	Machine *mach = &machine;
	Memory* mems = (Memory*)(mach->e820s_addr);
	size_t index = 0;

	//统计需要多少内存页
	for(uint_t i=0;i<mach->e820s_num;i++){
		//判断是否是可用内存
		if(RAM_USABLE==mems[i].type){
			//从内存段起始地址开始，4KB为一页
			for(addr_t adr=mems[i].addr; adr+0xfff<=mems[i].end; adr+=0x1000){
				index++;
			}
		}
	}

	//动态分配内存
	*pages = (Page*)allocate(index*sizeof(Page)); 
	return index;
}


void mempage_init(){
	Machine* mach = &machine;
	Memory* mems = (Memory*)(mach->e820s_addr);
	Page *pages = NULL;		//页描述符数组

	//创建pages数组
	size_t len = create_pages(&pages);

	//遍历额e820s数组,为所有可用内存建立页表
	for(uint_t i=0,index=0;i<mach->e820s_num;i++){
		//判断是否是可用内存
		if(RAM_USABLE==mems[i].type){
			//从内存段起始地址开始，4KB为一页，依次处理完
			for(addr_t adr=mems[i].addr; adr+0xfff<=mems[i].end; adr+=0x1000){
				//将页地址写入页描述符page
				set_page(&pages[index],adr);
				index++;
			}
		}
	}

	//填写机器信息结构体
	mach->pages_addr = (u64_t)pages;
	mach->pages_num = len;
	mach->pages_size = len*sizeof(Page);
	
	return;
}


void mempage_display(){
	Machine* mach = &machine;
	printk("pages address:0x%lx\n",mach->pages_addr);
	printk("pages number:0x%lx\n",mach->pages_num);
	printk("pages size:0x%lx\n",mach->pages_size);
	return;
}