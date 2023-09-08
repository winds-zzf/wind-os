/**
 * 内存初始化：将原来e820数组中的数据捯饬到Memory数组中，以提供更丰富的功能
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

static void memory_sort(Memory *mems,size_t size);
static bool_t memory_calloc(Machine *mach,Memory **mems,size_t* size);
static bool_t e820_to_memory(Memory* mem,E820* e820);
static void init_memory();

void memory_init(){
	//初始化内存信息
	init_memory();
	
	//初始化内存管理模块
	memmgr_init();
	
	return;
}

/**
 * 收集内存信息，生成Memory结构体
 */
static void init_memory(){
	Machine *mach = &machine;
	Memory *mems = NULL;	//Memory结构体数组
	size_t size = 0;		//数组大小
	uint_t num = 0;		//数组元素个数
	E820* e820s = (E820*)mach->e820s_addr;	//从机器信息结构体中获取e820结构体数组
	
	//动态内存分配
	if(!memory_calloc(mach,&mems,&size)){	//检查分配是否成功
		printk("Allocate the memory arrays error\n");
		return;
	}

	//依次将e820结构体中的数据拷贝到Memory结构体中去
	for(num=0;num<mach->e820s_num;num++){
		e820_to_memory(&mems[num],&e820s[num]);
	}
	
	//填写机器信息machine
	mach->e820s_addr = (addr_t)mems;
	mach->e820s_num = num;
	mach->e820s_size = size;
	
	//对Memory数组进行排序
	memory_sort(mems,mach->e820s_num);
	return;
}

void memory_display(){
	Machine* mach = &machine;
	printk("memory addr:0x%lx\n",mach->e820s_addr);
	printk("memory num:0x%lx\n",mach->e820s_num);
	printk("memory size:0x%lx\n",mach->e820s_size);
	Memory *mems = (Memory*)mach->e820s_addr;
	printk("address\t\tsize\t\tend\t\ttype\n");
	for(uint_t i=0;i<mach->e820s_num;i++){
		printk("0x%lx\t\t",mems[i].addr);
		printk("0x%lx\t\t",mems[i].size);
		printk("0x%lx\t\t",mems[i].end);
		printk("0x%lx\n",mems[i].type);
	}
	return;
}

/**
 * 交换两个Memory结构体变量的值
 * mem_a:Memory结构体指针
 * mem_b:Memory结构体指针
 */
static void memory_swap(Memory *mem_a,Memory *mem_b){
	Memory temp;		//创建一个临时结构体变量
	temp = *mem_a;		//把mem_a地址下的结构体赋值给temp;
	*mem_a = *mem_b;
	*mem_b = temp;
	return;
}

/**
 * 内存视图数组&选择排序&地址&升序排序
 * mems:内存视图结构体数组
 * size:内存视图结构体大小
 */
static void memory_sort(Memory *mems,size_t size){
	uint_t k;
	for(uint_t i=0;i<size-1;i++){
		k = i;
		for(uint_t j=i+1;j<size;j++){
			if(mems[j].addr<mems[k].addr){
				k=j;
			}
		}
		if(k!=i){
			memory_swap(&mems[i],&mems[k]);
		}
	}
	return ;
}

/**
 * 为Memory结构体数组分配内存地址，参考calloc(),malloc()
 * 这里没法直接定义结构体数组，因为e820_t数组的长度一个变量。
 * 同时C语言本身只有语法，并没有calloc和malloc这两个系统调用，所以需要手动内存分配
 */
static bool_t memory_calloc(Machine *mach,Memory **mems,size_t* size){
	if(NULL==mach||NULL==mach->e820s_addr||0==mach->e820s_size||0==mach->e820s_num){	//参数检查
		return FALSE;
	}
	*size = mach->e820s_num*sizeof(Memory);	//确定Memory的数量
	*mems = (Memory*)allocate(*size);		//为Memory结构体数组动态分配内存地址空间
	return TRUE;
}

/**
 * 将E820变量数据中转到Memory结构体变量中
 * mem: 单个Memory结构体变量
 * e820: 单个e820结构体变量
 */
static bool_t e820_to_memory(Memory* mem,E820* e820){
	if(NULL==mem||NULL==e820){	//参数检查
		return FALSE;
	}

	//初始化Memory结构体
	memset((addr_t)mem,0,sizeof(Memory));
	
	//复制参数
	mem->type = e820->type;	
	mem->addr = e820->addr;
	mem->size = e820->size;
	mem->end = e820->addr+e820->size-1;	//内存区域尾字节地址
	
	return TRUE;
}

