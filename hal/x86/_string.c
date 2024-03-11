/**
 * 字符串处理
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

sint_t strcmp(const char_t *src,const char_t *dst){
	while(*src && *dst && *src==*dst){	//在src和dst都没结束之前如果字符相同，则继续比较
		src++;
		dst++;
	}
	return *src - *dst;	//返回
}

size_t strcpy(char_t* const dst,const char_t *src){
	size_t i;
	for(i=0; 0!=src[i]; i++){
		dst[i] = src[i];
	}
	return i;
}

size_t strlen(const char *src){
	size_t len = 0;
	while(src[len]){
		len++;
	}
	return len;
}

size_t strcat(char_t* dst,const char_t* src){
	size_t size=0;
	while(*dst) {
		dst++;	//定位到dst字符串结尾
		size++;
	}
	while(*src){
		*dst++ = *src++;
		size++;
	}
	*dst = 0; 	//目标字符串写入结束符
	return size;
} 


//================================================================================================
void string_test_main(){
	
	
	
	die(0);
	return;
}