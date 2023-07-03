/**
 * 内存初始化
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

static void memory_sort(Memory *mems,size_t size);
static boolean_t memory_calloc(Machine *mach,Memory **mems,size_t* size);
static boolean_t e820_to_memory(Memory* mem,E820* e820);

void memory_init(Machine *mach){
	Memory *mems = NULL;	//Memory结构体数组
	size_t size = 0;		//数组大小
	uint_t num = 0;		//数组元素个数
	E820* e820s = (E820*)mach->e820_addr;

	//动态内存分配
	if(!memory_calloc(mach,&mems,&size)){	//检查分配是否成功
		printk("Allocate the memory arrays error\n");
		return;
	}

	//依次将e820结构体中的数据拷贝到Memory结构体中去
	for(num=0;num<mach->e820_num;num++){
		e820_to_memory(&mems[num],&e820s[num]);
	}
	
	//填写机器信息machine
	mach->e820_addr = (addr_t)mems;
	mach->e820_num = num;
	mach->e820_size = size;
	
	//对Memory数组进行排序
	memory_sort(mems,mach->e820_num);
	
	return;
}

void memory_display(Machine *mach){
	Memory *mems = (Memory*)mach->e820_addr;
	for(uint_t i=0;i<mach->e820_num;i++){
		printk("0x%x\n",mems[i].addr);
		printk("%d\n",mems[i].size);
		printk("0x%x\n",mems[i].end);
		printk("0x%x\n",mems[i].type);
	}
	return;
}

/**
 * 交换两个结构体变量的值
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
 * 对内存视图数组进行选择排序
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
 * 为Memory结构体数组分配内存地址
 * 这里没法直接定义结构体数组，因为e820_t数组的长度一个变量。
 * 同时C语言本身只有语法，并没有calloc和malloc函数，所以需要手动内存分配
 */
static boolean_t memory_calloc(Machine *mach,Memory **mems,size_t* size){
	if(NULL==mach||NULL==mach->e820_addr||0==mach->e820_size||0==mach->e820_num){	//参数检查
		return FALSE;
	}
	//*mems = (Memory*)allocate();		//将下一个4k对齐的地址作为Memory结构体数组的内存地址
	//*size = mach->e820_size;		//确定Memory的数量
	return TRUE;
}

/**
 * 将E820转换成Memory
 */
static boolean_t e820_to_memory(Memory* mem,E820* e820){
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
	
	return FALSE;
}
