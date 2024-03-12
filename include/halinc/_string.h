/**
 * 字符串处理
 * API定义
 */
#ifndef __STRING_H
#define __STRING_H

/**
 * 字符串比较,参考strcmp()
 * dst:源符串
 * src:目标字符串
 */
sint_t strcmp(const char *str1,const char_t *str2);


/**
 * 字符串拷贝,参考strcpy()
 * dst:源符串
 * src:目标字符串
 */
size_t strcpy(char* const dst,const char* src);


/**
 * 字符串长度计算，参考strlen()
 * src:待计算字符串
 */
size_t strlen(const char *src);

/**
 * 字符串连接，参考strcat()
 * dst: 源符串
 * src: 目标字符串
 */
size_t strcat(char_t* dst,const char_t* src);

/**
 * 内存初始化函数,以1子节单位,参考memset
 * addr: 待初始化的内存区域地址
 * value: 用于初始化的值
 * size: 初始化内存区域的大小
 */
INLINE void memset(void* addr,const u8_t val,const size_t size){
    if(NULL==addr||size<=0){
        return ;
    }
    u8_t* p = (u8_t*)addr;
    for(size_t i=0; i<size; i++){
        p[i] = val;
    }
}

/**
 * 内存拷贝函数，参考memcpy()
 * src: 拷贝数据源
 * size: 拷贝的字节数
 * dst: 拷贝目标地址
 */
INLINE size_t memcpy(void* dst,void* src,const size_t size){
	if(NULL==src||NULL==dst||size<=0){  //参数检查
		return 0;
	}
	u8_t* p_src = (u8_t*) src;
	u8_t* p_dst = (u8_t*) dst;
	
	for(size_t i=0;i<size;i++){
		//逐子节拷贝
		*p_dst++ = *p_src++;
	}
	return size;
}


#endif	//__STRING_H