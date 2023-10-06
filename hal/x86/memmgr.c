/**
 * 内存管理模块
 */

#include "globaltype.h"
#include "globalctrl.h"

static void set_memmgr(Memmgr *mgr);

void memmgr_init(Machine* mach){
	Memmgr *mgr = &memmgr;	//obtaining the handle of memmgr
	
	//重新初始化页表(此处可以更改分页模式)
	remmu_init();

	//1.initializing memviews
	memview_init();
	
	//2.initializing pages
	mempage_init();
	//test_mempage_main();

	//3.initializing memory areas
	memarea_init();
	//test_memarea_main()

	//4.marking occupied pages（this step must follow memarea_init）
	mempage_occupy();
	//test_mempage_main();

	//5.mark and mount pages to areas（内核占用的内存页不参与内存分配，因此并不会被挂载上去）
	mount_pages(mgr->areas);

	//6.further initializing for 'mgr'
	set_memmgr(mgr);

	//7.initializing the memory object
	memobject_init();
	//memobject_test_main();

	
	//测试API
	//memapi_test_main();
	return;
}

void memmgr_display(Machine *mach){
	
	return;	
}

/**
 * simple initialization for 'Memmgr' struct
 */
static void init_memmgr(Memmgr *mgr){
	//
	list_init(&mgr->list);
	spinlock_init(&mgr->lock);
	//
	mgr->status = 0;
	mgr->flag = 0;
	//
	mgr->max_pages = 0;
	mgr->free_pages = 0;
	mgr->allocate_pages = 0;
	mgr->retain_pages = 0;
	mgr->horizon = 0;
	mgr->memsize = 0;
	//初始化内存视图，内存页，内存区
	mgr->views = NULL;
	mgr->views_num = 0;
	mgr->pages = NULL;
	mgr->pages_num = 0;
	mgr->areas = NULL;
	mgr->areas_num = 0;
	
	return;
}

/**
 * further initializing for memmgr 
 */
static void set_memmgr(Memmgr *mgr){
	Machine *mach = &machine;
	//basic initializing for memmgr
	init_memmgr(mgr);
	//inheriting the physical addresses about memmgr from machine and converting them to virtual  
	mgr->views = (MemView*)mach->e820s_addr;	//virtual address
	mgr->views_num = mach->e820s_num;
	mgr->pages = (MemPage*)mach->pages_addr;	//virtual address
	mgr->pages_num = mach->pages_num;
	mgr->areas = (MemArea*)mach->areas_addr;	//virtual address
	mgr->areas_num = mach->areas_num; 

	//the size of memory which is descripted by page
	mgr->memsize = mach->pages_num<<12;	//4K page in size
	mgr->max_pages = mach->pages_num;
	
	//count all allocated pages
	for(size_t i=0;i<mach->pages_num;i++){
		if(1==mgr->pages[i].flags.count
		&&PAGEFLAGS_MOCTY_KRNL==mgr->pages[i].flags.mocty
		&&PAGEADDR_ALLOC == mgr->pages[i].addr.allocate){
			mgr->allocate_pages++;
		}
	}
	//count all free pages
	mgr->free_pages = mgr->max_pages-mgr->allocate_pages;

	return ;
}
