/**
 * 内存区初始化
 */

#include "globaltype.h"
#include "globalctrl.h"

static void init_memarea(MemArea *areas);


void memarea_init(){
	/**
	 * 运算符取地址的时候，成员的地址是List地址+成员偏移地址，因此如果List就是物理地址，那么成员地址也是物理地址
	 * 因此为了避免areas及其成员地址为物理地址，这里areas应该使用虚拟地址
	 */
	MemArea *areas = (MemArea*)MAP_RV(machine.next_addr);
	
	//initializing four areas sequentially
	for(uint_t i=0;i<MEMAREA_MAX;i++){
		init_memarea(&areas[i]);
	}
	//setting the type and size of hardware area
	areas[0].type = MEMAREA_TYPE_HWAD;
	areas[0].start = MEMAREA_HWAD_START;
	areas[0].end = MEMAREA_HWAD_END;
	areas[0].size = MEMAREA_HWAD_SIZE;
	//setting the type and size of kernel area
	areas[1].type = MEMAREA_TYPE_KRNL;
	areas[1].start = MEMAREA_KRNL_START;
	areas[1].end = MEMAREA_KRNL_END;
	areas[1].size = MEMAREA_KRNL_SIZE;
	//setting the type and size of app area
	areas[2].type = MEMAREA_TYPE_APPS;
	areas[2].start = MEMAREA_APPS_START;
	areas[2].end = MEMAREA_APPS_END;
	areas[2].size = MEMAREA_APPS_SIZE;
	//setting the type and size of shared area.(undefined)
	areas[3].type = MEMAREA_TYPE_SHAR;
	areas[3].start = 0;
	areas[3].start = 0;
	areas[3].start = 0;
	
	//fill the relative machine field out
	machine.areas_addr = (u64_t)areas;
	machine.areas_num = (u64_t)MEMAREA_MAX;
	machine.areas_size = (u64_t)(sizeof(MemArea)*MEMAREA_MAX);
	//updating the next_addr to next page aligned 4K 
	machine.next_addr = ALIGN_4K(machine.next_addr+machine.areas_size);

	return ;
}

/**
 * 遍历所有页描述符，找出属于area的页,并对其进行标记
 */
static void mark_area(MemArea area,MemPage* pages,size_t len){
	addr_t addr;	//页物理地址
	//遍历整个页表
	for(uint_t i=0;i<len;i++){
		//只标记未标记过的
		if(MEMAREA_TYPE_INIT==pages[i].flags.marty){	
			//获取页表中记录的页物理地址
			addr = pages[i].addr.value<<12;
			//如果这个页地址在该内存区中，则执行标记
			if(area.start<=addr && (addr+0xfff)<=area.end){
				//设置page的标签
				pages[i].flags.marty = area.type&0x3;
			}
		}
	}
	
	return;
}


/**
 * 标记所有页所属的区，并将其挂载到对应内存区
 */
void mount_pages(){
	//get memory page table
	MemPage* pages = (MemPage*)machine.pages_addr;
	size_t pages_len = machine.pages_num;
	//get memory areas array
	MemArea *areas = (MemArea*)machine.areas_addr;
	size_t areas_len = machine.areas_num;
	
	//the first traversal: 标记内存页所属的内存区(确定内存页属于哪个区)
	for(size_t i=0;i<areas_len;i++){
		//针对一个areas给pages打上标签
		mark_area(areas[i],pages,pages_len);
	}
	
	//the second traversal: 将内存页挂载到所属内存区
	for(size_t i=0;i<areas_len;i++){
		//初始化内存表结构体
		memtable_init(&areas[i]);
		//针对一个areas对pages进行合并
		mount_table(&areas[i],pages,pages_len);
	}
	
	return;
}


/**
 * basic initialization for single MemArea instance 
 */
static void init_memarea(MemArea *area){
	list_init(&area->list);
	spinlock_init(&area->lock);
	area->status = 0;
	area->flag = 0;
	area->type = MEMAREA_TYPE_INIT;
	area->total_num = 0;
	area->free_num = 0;
	area->retain_num = 0;
	area->horizon = 0;
	area->start = 0;
	area->end = 0;
	area->size = 0;
	area->allocate_num = 0;
	
	return;
}

//==============================================================================================

INLINE void test_memareas(MemArea *areas);
INLINE void test_mark();

void memarea_test_main(){
	
	return;
}

//test initialization of memory
INLINE void test_memareas(MemArea *areas){
	for(uint_t i=0;i<MEMAREA_MAX;i++){
		bool_t result = (areas[i].start+areas[i].size-1)==areas[i].end;
		printk("0x%lx\t0x%lx\t0x%lx\t0x%lx\t%d\n",areas[i].type,areas[i].start,areas[i].size,areas[i].end,result);
	}
	return;
}

//test the reault of 'marking areas'
INLINE void test_mark(){
	MemPage* pages = (MemPage*)machine.pages_addr;
	size_t len = machine.pages_num;
	for(size_t i = 0;i<len;){
		switch(pages[i].flags.marty){	//循环计数变量由内部子循环控制
		case MEMAREA_TYPE_INIT:	//输出物理页号
			printk("INIT:0x%lx,",pages[i].addr.value);
			while(i<len && MEMAREA_TYPE_INIT==pages[i].flags.marty) i++;
			printk("0x%lx\n",pages[i-1].addr.value);
			break;
		case MEMAREA_TYPE_HWAD:
			printk("HWAD:0x%lx,",pages[i].addr.value);
			while(i<len && MEMAREA_TYPE_HWAD==pages[i].flags.marty) i++;
			printk("0x%lx\n",pages[i-1].addr.value);
			break;
		case MEMAREA_TYPE_KRNL:
			printk("KRNL:0x%lx,",pages[i].addr.value);
			while(i<len && MEMAREA_TYPE_KRNL==pages[i].flags.marty) i++;
			printk("0x%lx\n",pages[i-1].addr.value);
			break;
		case MEMAREA_TYPE_APPS:
			printk("APPS:0x%lx,",pages[i].addr.value);
			while(i<len && MEMAREA_TYPE_APPS==pages[i].flags.marty) i++;
			printk("0x%lx\n",pages[i-1].addr.value);
			break;
		case MEMAREA_TYPE_SHAR:
			printk("SHAR:0x%lx,",pages[i].addr.value);
			while(i<len && MEMAREA_TYPE_SHAR==pages[i].flags.marty) i++;
			printk("0x%lx\n",pages[i-1].addr.value);
			break;
		}
	}
	return;
}

