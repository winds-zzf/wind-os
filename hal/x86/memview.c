/**
 * 内存初始化：将原来e820数组中的数据捯饬到Memory数组中，以提供更丰富的功能
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

static void memviews_sort(MemView *views,size_t size);
static bool_t e820_to_memview(MemView *view,E820 *e820);

/**
 * 收集内存信息，生成Memory结构体
 */
void memview_init(){
	machine_t *mach = &machine;
	uint_t num = 0;		//数组元素个数
	E820* e820s = (E820*)mach->e820s_addr;	//从机器信息结构体中获取e820结构体数组
	
	//因为next_addr是物理地址，所以首次初始化的时候，要转换成虚拟地址，保证mach->e820s_addr是虚拟地址
	MemView* views = (MemView*)MAP_RV(mach->next_addr);
	
	//依次将e820结构体中的数据拷贝到Memory结构体中去
	for(num=0;num<mach->e820s_num;num++){
		e820_to_memview(&views[num],&e820s[num]);
	}

	//对Memory数组进行排序
	memviews_sort(views,num);
	
	//填写机器信息machine
	mach->e820s_addr = (addr_t)views;
	mach->e820s_num = num;
	mach->e820s_size = num*sizeof(MemView);
	//计算下一个4K对齐地址
	mach->next_addr = ALIGN_4K(mach->next_addr+mach->e820s_size);

	return;
}

void memview_display(){
	machine_t* mach = &machine;
	printk("memory addr:0x%lx\n",mach->e820s_addr);
	printk("memory num:0x%lx\n",mach->e820s_num);
	printk("memory size:0x%lx\n",mach->e820s_size);
	MemView *views = (MemView*)mach->e820s_addr;
	printk("address\t\tsize\t\tend\t\ttype\n");
	for(uint_t i=0;i<mach->e820s_num;i++){
		printk("0x%lx\t\t",views[i].addr);
		printk("0x%lx\t\t",views[i].size);
		printk("0x%lx\t\t",views[i].end);
		printk("0x%lx\n",views[i].type);
	}
	return;
}

/**
 * 交换两个Memview结构体变量的值
 * view_a:MemView结构体指针
 * view_b:MemView结构体指针
 */
static void memview_swap(MemView *view_a,MemView *view_b){
	MemView temp;		//创建一个临时结构体变量
	temp = *view_a;		//把mem_a地址下的结构体赋值给temp;
	*view_a = *view_b;
	*view_b = temp;
	return;
}

/**
 * 内存视图数组&选择排序&地址&升序排序
 * views:内存视图结构体数组
 * size:内存视图结构体大小
 */
static void memviews_sort(MemView *views,size_t size){
	uint_t k;
	for(uint_t i=0;i<size-1;i++){
		k = i;
		for(uint_t j=i+1;j<size;j++){
			if(views[j].addr<views[k].addr){
				k=j;
			}
		}
		if(k!=i){
			memview_swap(&views[i],&views[k]);
		}
	}
	return ;
}

/**
 * 将E820变量数据中转到Memory结构体变量中
 * mem: 单个Memory结构体变量
 * e820: 单个e820结构体变量
 */
static bool_t e820_to_memview(MemView* view,E820* e820){
	if(NULL==view||NULL==e820){	//参数检查
		return FALSE;
	}

	//初始化Memory结构体
	memset(view,0,sizeof(MemView));
	
	//复制参数
	view->type = e820->type;	
	view->addr = e820->addr;
	view->size = e820->size;
	view->end = e820->addr+e820->size-1;	//内存区域尾字节地址
	
	return TRUE;
}

