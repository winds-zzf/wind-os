/**
 * 工具模块
 * API实现
 */
#include"lib.h"
#include"utils_t.h"

u64_t utf_to_unicode(const u64_t utf8){
	
	return 0;
}

u64_t unicode_to_utf8(const u64_t unicode){
	
	return 0;
}

u32_t checksum_crc32(addr_t addr,size_t size,u32_t poly){
	u32_t crc = 0xFFFFFFFFU;
	u8_t* data = (u8_t*)addr;
	for (size_t i = 0; i < size; i++) {
		crc ^= data[i];
		for (size_t j = 0; j < 8; j++) {
			if (crc & 1)
				crc = (crc >> 1) ^ poly;
			else
				crc >>= 1;
		}
	}
	crc ^= 0xFFFFFFFFU;
	return crc;
}

u64_t checksum_crc64(addr_t addr,size_t size,u64_t poly){
	u64_t crc = 0xFFFFFFFFFFFFFFFFULL;
	u8_t* data = (u8_t*)addr;
	for (size_t i = 0; i < size; i++) {
		crc ^= data[i];
		for (size_t j = 0; j < 8; j++) {
			if (crc & 1)
				crc = (crc >> 1) ^ poly;
			else
				crc >>= 1;
		}
	}
	crc ^= 0xFFFFFFFFFFFFFFFFULL;
	return crc;
}

void error(const char_t* code,const char_t* pos,const char_t *msg){
	printk("error [%s] @<%s> : %s\n",code,pos,msg);
	HALT();	//主动死机
}

void warning(const char_t *code,const char_t *pos,const char_t *msg){
	printk("warning [%s] @<%s> : %s\n",code,pos,msg);
	return;
}

//取消编译器优化，增强延时效果
#pragma GCC push_options
#pragma GCC optimize("O0")
void die(uint_t delay){
	if(0==delay){
		while(TRUE);
	}
	for(uint_t i=0;i<delay;i++){
		for(uint_t j=0;j<delay;j++){
			for(uint_t k=0;k<delay;k++){
				;
			}
		}
	}

	return;
}
#pragma GCC pop_options
