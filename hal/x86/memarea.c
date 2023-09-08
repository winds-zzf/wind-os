/**
 * 内存区初始化
 */

#include "globaltype.h"
#include "globalctrl.h"

/**
 * 标记占用的内存区域为“已分配”
 * addr: 
 * size: 
 */
static void mark_occupy(addr_t addr,size_t size){
	
}


/**
 * 标记那些手动分配的内存占用的内存页
 * 
 * 注意allocate如果能够管理第一个块内存段，那么可以直接通过遍历bitmap的方法标记已用内存
 */
static void init_occupy(){
	Machine* mach = &machine;

	//内核栈占用内存
	mark_occupy(mach->stack_addr,mach->stack_size);
	//mmu占用内存
	mark_occppy(mach->page_addr,mach->page_size);
	//e820s数组占用内存
	mark_occupy(mach->e820s_addr,mach->e820s_size);
	//font占用内存
	mark_occupy(mach->font_addr,mach->font_size);
	//内核文件占用内存
	mark_occupy(mach->kernel_addr,mach->kernel_size);
	//内存页表占用内存
	mark_occupy(mach->pages_addr,mach->pages_size);
	//位示图占用内存
	mark_occupy(mach->bitmap_addr,mach->bitmap_size);
	
	return;
}

/**
 * 新版的内存标记函数
 */
static void init_occupy_new(){
	return;
}


static void init_memarea(MemArea *area){
	list_init(&area->list);
	knl_spinlock_init(&area->lock);
	area->status = 0;
	area->flag = 0;
	area->type = MEMAREA_TYPE_INIT;
	area->total_num = 0;
	area->allocate_num = 0;
	area->free_num = 0;
	area->retain_num = 0;
	area->horizon = 0;
	area->start = 0;
	area->end = 0;
	area->size = 0;
	memtable_init(&area->table);
	return;
}

void memarea_init(){
	//依次简单初始化四个内存区
	for(uint_t i=0;i<MEMAREA_MAX;i++){
		init_memarea(&memAreas[i]);
	}
	//设置硬件区的类型和大小
	memAreas[0].type = MEMAREA_TYPE_HWAD;
	memAreas[0].start = MEMAREA_HWAD_START;
	memAreas[0].end = MEMAREA_HWAD_END;
	memAreas[0].size = MEMAREA_HWAD_SIZE;
	//设置内核区的类型和大小
	memAreas[1].type = MEMAREA_TYPE_KRNL;
	memAreas[1].start = MEMAREA_KRNL_START;
	memAreas[1].end = MEMAREA_KRNL_END;
	memAreas[1].size = MEMAREA_KRNL_SIZE;
	//设置应用区的类型和大小
	memAreas[2].type = MEMAREA_TYPE_APPS;
	memAreas[2].start = MEMAREA_APPS_START;
	memAreas[2].end = MEMAREA_APPS_END;
	memAreas[2].size = MEMAREA_APPS_SIZE;
	
	return ;
}

void memarea_display(){
	return;
}
