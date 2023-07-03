/**
 * 简单内存管理(位示图,低地址优先)
 * API实现
 */
#include"lib.h"
#include"allocate_t.h"

static addr_t memory_start = 0x100000;	//内存起始地址
static size_t memory_size = 0x8000000;	//内存大小
static size_t block_num = 8000;		//块数
static u32_t *maps = NULL;			//位示图

/**
 * 置位'|1'
 */
static void sets(size_t num, const size_t count) {
	printk("0x%x,%d\n",num*BLOCK_SIZE,count);
	for (size_t i = 0; i < count; i++) {
		maps[num / 32] |= ATTR >> (num % 32);
		num++;	//指向下一块
	}
	return;
}

/**
 * 复位'&0'
 */
static void resets(size_t num, const size_t count) {
	printk("0x%x,%d\n",num*BLOCK_SIZE,count);
	for (size_t i = 0; i < count; i++) {
		maps[num / 32] &= ~(ATTR >> (num % 32));
		num++;	//指向下一块
	}
	return;
}

addr_t allocate(const size_t size) {
	size_t total = GET_SIZE(size);		//获取所需的块数
	for (size_t i = 0, count = 0; i < block_num/32; i++) {
		for (size_t j = 0; j < 32; j++) {
			if ((maps[i]<<j)&ATTR) {	//跳过已占用数据块
				count = 0;			//重新开始计数
			}
			else {
				count++;	//空白块加一
				if (count == total) {	//空白块数达到要求
					sets(i * 32 + j + 1 - total, total);		//置位
					allocate_display();	//显示位示图
					return GET_ADR(i * 32 + j + 1 - total)+memory_start;	//返回实际地址
				}
			}
		}
	}
	return NULL;	//查找失败
}

void reserve(const addr_t addr, const size_t size) {
	if(addr<memory_start) return;	//参数检查
	sets(GET_NUM(addr-memory_start),GET_SIZE(size));	//置位
	allocate_display();
	return;			//置位
}

void release(const addr_t addr, const size_t size) {
	if(addr<memory_start) return;	//参数检查
	resets(GET_NUM(addr-memory_start), GET_SIZE(size));	//复位
	allocate_display();
	return;
}

void allocate_init(addr_t maps_addr,addr_t mem_addr,size_t mem_size){
	maps = (u32_t*)maps_addr;	//比特图
	memory_start = mem_addr;
	memory_size = mem_size;
	block_num = mem_size/BLOCK_SIZE;	//求内存块数
	return;
}

/**
 * 打印出32位数据的二进制数
 */
static void print_bin(u32_t val) {
	char_t buffer[33];
	for (size_t i = 0; i < 32; i++) {
		buffer[31 - i] = "01"[(val >> i) & 1];	//取出指定位
	}
	buffer[32] = 0;	//结束符

	for(uint_t i=0;i<32;i++){
		if(i!=0&&i%8==0) printk(" ");
		printk("%c",buffer[i]);
	}
	printk("\n");
	return;
}

/**
 * 打印出地址的16进制形式
 */
static void print_hex(addr_t val) {
	char_t buffer[17];
	size_t width = 8*sizeof(addr_t);	//计算地址位数
	for (size_t i = 0; i < width; i+=4) {
		buffer[(width-i-1)/4] = "0123456789ABCDEF"[(val >> i) & 0xF];
	}
	buffer[width/4] = 0;	//结束符
	printk("0x%s:",buffer);
	return;
}

void allocate_display() {
	for (size_t i = 0; i < block_num/32; i++) {
		if (0!=maps[i]) {
			print_hex(memory_start+ i * BLOCK_SIZE * 32);
			print_bin(maps[i]);
		}
	}
	return;
}
