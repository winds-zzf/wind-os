/**
 * e820数组 
 * API实现
 */
#include"global.h"
#include"e820_t.h"

static void get_e820s(E820** e820s,size_t *size);
static u64_t get_memsize(E820* e820s,size_t size);
static E820* check_memsize(E820* e820s,size_t num,addr_t adr,size_t size);

void e820_init(Machine* mach){
	E820 *e820s = NULL;		//e820数组地址
	size_t num = 0;		//e820数组元素个数
	
	//获取e820数组(数据和大小)
	get_e820s(&e820s,&num);	
	if(0==num){
		printk("there is no e820 array in your computer!\n");
	}

	//检查指定内存地址区域是否可用(内存地址从1MB开始大小为128MB的区域是USABLE)，0x0~0xfffff这1MB内存段一般不使用
	if(NULL == check_memsize(e820s,num,0x100000U,0x8000000U)){	//找不到满足条件的内存段
		printk("Your computer is low on memory,the usable memory isn't less than 128MB\n");
	}

	//复制e820到指定位置
	addr_t dst = allocate(num*sizeof(E820));		//内存分配
	memcpy((addr_t)e820s,num*sizeof(E820),dst);	//拷贝到新分配位置
	
	//填写机器信息
	mach->e820s_addr = (u64_t)dst;	//32位的指针需要先转换为32位的常数，再转换为64位的数
	mach->e820s_num = num;	//e820数组元素个数
	mach->e820s_size = num*sizeof(E820);	//e820s数组大小
	mach->mem_size = get_memsize(e820s,num);	//可用内存总大小
	return ;
}

void e820_display(Machine mach){
	printk("=================================e820 info==================================\n");
	E820 *e820s = (E820 *)((addr_t)mach.e820s_addr);
	uint_t size = (size_t)mach.e820s_num;
	for(uint_t i=0;i<size;i++){
		printk("%d\t",i);
		printk("0x%lx\t\t",e820s[i].addr);
		printk("0x%lx\t\t",e820s[i].size);
		printk("%d\n",e820s[i].type);
	}
	printk("memory size:0x%lx\n",mach.mem_size);
	printk("=================================e820 end.===================================\n");
	return ;
}

/**
 * 通过调用实模式BIOS中断获取内存视图的e820数组
 * e820s:e820数组的地址
 * num:e820数组元素数量
 */
static void get_e820s(E820** e820s,size_t *size){
	real_entry(0,0,0);		//进入实模式调用BIOS中断，获取e820数组
	*size = *((addr_t*)E820_SIZE);		//取出E820_ADR地址下存储的E820数组地址值
	*e820s = (E820*)(*((addr_t*)E820_PADR));	//取出E820_SIZE地址下存储的E820大小
	return;
}

/**
 * 获取可用内存(USEABLE)大小
 * 遍历e820数组，求出各个可用内存区域(USABSE)的大小总和
 * e820s:e820数组的地址
 * num:e820数组元素数量
 */
static u64_t get_memsize(E820* e820s,size_t size){
	u64_t sum = 0;
	if(NULL==e820s||0==size){		//参数检查
		return 0;
	}
	for(size_t i=0;i<size;i++){
		if(RAM_USABLE == e820s[i].type){		//可用内存
			sum += e820s[i].size;
		}
	}
	return sum;
}

/**
 * 检查可用内存段是否存在包含指定地址范围的可用内存块
 * e820s:e820数组的地址
 * num:e820数组元素数量
 * adr:指定内存块的起始地址
 * size:指定内存块的大小
 */
static E820* check_memsize(E820* e820s,size_t num,addr_t adr,size_t size){
	u64_t adr_start = adr;
	u64_t adr_end = adr+size;
	if(NULL==e820s||0==size){		//参数检查
		return 0;
	}
	//查找能包含指定内存区域的e820可用内存段[e820.start,e820.end]包含[adr_start,adr_end]
	for(size_t i=0;i<num;i++){
		if(RAM_USABLE==e820s[i].type && e820s[i].addr<=adr_start && adr_end<=e820s[i].addr+e820s[i].size){
			return (e820s+i);
		}
	}
	
	return NULL;	//查找失败
}
