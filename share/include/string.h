/**
 * 字符串处理
 * API定义
 */
#ifndef __STRING_H
#define __STRING_H

/**
 * 字符串比较
 * dst:源符串
 * src:目标字符串
 */
sint_t strcmp(const char *src,const char_t *dst);


/**
 * 字符串拷贝
 * dst:源符串
 * src:目标字符串
 */
size_t strcpy(char* const dst,const char* src);


/**
 * 字符串长度计算
 * src:待计算字符串
 */
size_t strlen(const char *src);

/**
 * 字符串连接
 * dst:源符串
 * src:目标字符串
 */
size_t strcat(char_t* dst,const char_t* src);

/**
 * 内存清零函数
 * addr:
 * value:
 * size
 */
INLINE size_t memset(const addr_t addr,const u8_t val,const size_t size){
    if(NULL==addr||size<=0){
        return 0;
    }
    u8_t* p = (u8_t*)addr;
    for(size_t i=0; i<size; i++){
        p[i] = val;
    }
    return size;
}

/**
 * 内存拷贝函数
 * src:拷贝数据源
 * size:拷贝的字节数
 * dst:拷贝目标地址
 */
INLINE size_t memcpy(const addr_t src,const size_t size,addr_t const dst){
    if(NULL==src||NULL==dst||size<=0){  //参数检查
        return 0;
    }
    u8_t* p_src = (u8_t*) src;
    u8_t* p_dst = (u8_t*) dst;
    
    for(size_t i=0;i<size;i++){	//逐子节拷贝
       *p_dst++ = *p_src++;
    }
    return size;
}

#endif	//__STRING_H