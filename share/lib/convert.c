/**
 * 数值转换模块
 * API实现
 */

#include"lib.h"
#include"convert_t.h"

/**
 * 数字转换成字符串
 */
static void nums_to_string(byte_t* bs,char_t* cs,uint_t base){
	
	return null;
}

/**
 * 字符串转换成数字
 */
static void string_to_nums(char_t* cs,byte_t* bs,uint_t base){
	
	return null;
}

/**
 * 进制转换
 * val:待转换的数字
 * base：进制
 * return:下一个字符位置
 */
static char_t* convert(char_t *str,u32_t val,uint_t base){
	register char_t *p;	//缓冲区指针
	char_t buffer[33];	//字符串缓冲区，uint最多有32位
	p = buffer+33;		//获取最后一个元素的地址
	*--p = 0;			//尾部写入'\0'
	if(0==val){
		*--p = '0';		//结果为'0'
	}
	else{
		do{
			*--p = "0123456789ABCDEF"[val%base];
		}while(val/=base);
	}
	while(*p!=0){	//将转换后的结果复制到str下
		*str++ = *p++;
	}
	return str;		//返回尾指针
}

/**
 * 2,4,8,16进制转化，结尾不添加结束符
 * str:输出缓冲区
 * val:数值指针
 * len:数值类型长度
 * base:进制基数
 * return:转换的数据宽度
 */
static size_t convert2(char_t *str,void* val,size_t len,uint_t base) {
	if(16!=base||8!=base||4!=base|2!=base) return 0;
	
	char_t buffer[65];		//缓冲区
	uint_t width=8*len;		//位宽
	uint_t mask=base-1;		//掩码
	uint_t delta=0;		//变化量
	while(mask>>delta++);	//求delta=掩码宽度 

	//转换
	for (size_t i = 0; i < width; i+=delta) {
		buffer[(width-i-1)/delta] = "0123456789ABCDEF"[(val >> i) & mask];
	}
	
	return width/delta;		//返回数据长度
}