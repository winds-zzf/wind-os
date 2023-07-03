/**
 * 字符串处理
 * API实现
 */
#include"lib.h"
#include"stdio_t.h"
#include"vgastr.h"

/**
 * 进制转换
 * val:待转换的数字
 * base：进制
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
	return str;		//返回尾指针：下一个字符位置
}

/**
 * 字符串拷贝函数
 * 将字符串从src拷贝到buf指向的地址下，并返回尾指针
 */
static char_t* _strcpy(char *buf,const char_t *src){
	while(*src){
		*buf++ = *src++;
	}
	return buf;
}


/**
 * 缓冲区格式化输入函数
 * buffer:输入缓冲区
 * fmt:
 * args:
 */
static void _sscank(char_t* buffer,const char_t* fmt,va_list args){
	char_t* p = buffer;     // 输入缓冲区指针

    	while (*fmt) {
        	if ('%' != *fmt) {
            	p++;        // 跳过非控制字符
            	fmt++;      // 处理下一个字符
            	continue;
        	}

       	fmt++;      // 跳过 '%'
        	char_t* dst = (char_t*)va_arg(args, addr_t);   // 获取下一个地址

        	switch (*fmt) {
        	case 's':   // 字符串
         		while (' ' != *p && '\n' != *p && '\t' != *p) {
         			*dst++ = *p++;     // 复制数据
         		}
           	*dst = '\0';   // 写入字符串结束符
         		break;
         		
     	case 'c':   // 字符
          	*dst = *p++;
          	break;
          	
         	case 'x':   // 十六进制
       	case 'd':   // 十进制
       	case 'o':   // 八进制
        	case 'b':   // 二进制
       		break;
       		
      	default:
          	// 处理未知格式指示符
         		break;
		}
		
		fmt++;      // 处理下一个格式指示符	
	}
	return;
}


void sscank(char_t* buffer,const char_t* fmt,...){
	va_list ap;
	va_start(ap,fmt);
	_sscank(buffer,fmt,ap);	//调用_sscank()将格式化数据填充到buffer中
	va_end(ap);
	return;
}

void scank(const char_t* fmt,...){
	char_t buffer[BUFFER_SIZE];	//输入缓冲区不可共享，因此不能够是static或global的
	//read_string(buffer);		//将字符串读取到缓冲区
	va_list ap;
	va_start(ap,fmt);
	_sscank(buffer,fmt,ap);
	va_end(ap);
	return ;
}


/**
 * 缓冲区格式化输出函数
 * buffer:输出缓冲区
 * fmt
 * args
 */
static void _sprintk(char_t* buffer,const char_t* fmt,va_list args){
	char_t *p = buffer;	//获取字符串指针
	while(*fmt){		//依次处理每个字符
		if('%'!=*fmt){	//处理常规字符
			*p++ = *fmt++;
			continue;
		}
		fmt++;	//跳过'%'并开始处理
		switch(*fmt++){
		case 's':	//string
			p = _strcpy(p,(char_t*)va_arg(args,char_t*));
			break;
		case 'c':	//char
			*p++ = (char_t)va_arg(args,uint_t);
			break;
		case 'x':	//16 
			p = convert(p,va_arg(args,u32_t),16);
			break;
		case 'd':	//10
			p = convert(p,va_arg(args,u32_t),10);
			break;
		case 'o':	//8
			p = convert(p,va_arg(args,u32_t),8);
			break;
		case 'b':	//2
			p = convert(p,va_arg(args,u32_t),2);
			break;
		default:	//
			break;
		}
	}
	*p++ = 0;	//添加结束符
	return;
}

void sprintk(char_t* buffer,const char_t* fmt,...){
	va_list ap;
	va_start(ap,fmt);
	_sprintk(buffer,fmt,ap);
	va_end(ap);
	return;
}

void printk(const char_t* fmt,...){
	char_t buffer[BUFFER_SIZE];		//字符缓冲区
	va_list ap;
	va_start(ap,fmt);
	_sprintk(buffer,fmt,ap);			//调用sprintk，进行字符串格式化
	va_end(ap); 
	write_string((const char_t*)buffer);		//将字符串输出到屏幕上
	return;
}
