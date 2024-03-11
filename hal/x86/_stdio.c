/**
 * 字符串处理
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

/**
 * 补码转换为原码
 */

/**
 * float解析
 */


/**
 * double解析
 */



/**
 * 通用R进制转换
 * str: 转换结果存储在str指向的字符串中
 * val: 待转换的数字,这里设置成addr_t类型，是为了适应CPU模式的改变(32位/64位)
 * base: 进制基数R
 */
static char_t* convertr(char_t *str,addr_t val,uint_t base){
	register char_t *p;	//工作指针，p指向所得字符串的首字符
	char_t buffer[65];	//字符串缓冲区，long long最多有64位
	p = buffer+65;		//获取最后一个元素的地址
	*--p = 0;			//尾部写入'\0'
	
	do{
		*--p = "0123456789ABCDEF"[val%base];
	}while(val/=base);	//32位CPU下不支持64位的除法和求余操作
	
	while(0!=*p){		//将转换后的结果复制到str下
		*str++ = *p++;
	}
	
	return str;		//返回尾指针：下一个字符位置
}

/**
 * 仅支持2^r进制的转换，这种情况下比通用进制转换效率更高
 * str: 转换结果存储在str指向的字符串中
 * val: 待转换的64位无符号整数
 * order: 2的阶数，如8进制的order=3
 */
static char_t* convert2(char_t *str,u64_t val,uint_t order){
	register char_t *p;
	char_t buffer[65];
	p = buffer + 65;
	*--p = 0;
	
	do{
		*--p ="0123456789ABCDEF"[val&((0x1<<order)-1)];
	}while(val>>=order);		
	
	while(0!=*p){
		*str++ = *p++;
	}
	return str;
}

/**
 * 字符串拷贝函数
 * 将字符串从src拷贝到buf指向的地址下，并返回尾指针
 */
static char_t* _strcpy(char_t *buf,const char_t *src){
	while(*src){
		*buf++ = *src++;
	}
	return buf;
}


/**
 * 缓冲区格式化输入函数
 * buffer:输入缓冲区
 * fmt: 格式控制字符串
 * args: 参数列表
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
 * buffer: 输出缓冲区
 * fmt: 格式控制字符串
 * args: 参数列表
 * 缺少对参数个数与格式控制字符串是否匹配的检查，
 * 不支持十进制负整数，十进制正负小数的输出
 */
static void _sprintk(char_t* buffer,const char_t* fmt,va_list args){
	char_t *p = buffer;	//位置指针
	
	while(*fmt){		//依次处理每个字符
		if('%'!=*fmt){	//处理常规字符
			*p++ = *fmt++;
			continue;
		}else fmt++;	//跳过'%'并开始处理

		//处理对齐方式
		/*
		char_t align = '+';	//对齐方式(默认左对齐)
		if('-'==*fmt){
			align = '-';
			fmt++;
		}
		*/
		
		//处理位宽
		/*
		uint_t width = 0;	//位宽大小(默认0)
		while('0'<=*fmt && *fmt<='9'){	//数字字符
			width = width*10 + (*fmt-'0');
			fmt++;	//指针后移){
		}
		*/

		//处理具体格式
		switch(*fmt++){
		case 's':	//string
			p = _strcpy(p,(char_t*)va_arg(args,char_t*));
			break;
		case 'c':	//char
			*p++ = (char_t)va_arg(args,uint_t);
			break;
		case 'x':	//16进制,32bits
			p = convert2(p,(u64_t)va_arg(args,u32_t),4);
			break;
		case 'd':	//10进制,32bits
			p = convertr(p,(addr_t)va_arg(args,u32_t),10);
			break;
		case 'o':	//8进制,32bits
			p = convert2(p,(u64_t)va_arg(args,u32_t),3);
			break;
		case 'b':	//2进制,32bits
			p = convert2(p,(u64_t)va_arg(args,u32_t),1);
			break;
		case 'l': //长类型
			switch(*fmt++){
				case 'x':	//16进制,64bits
					p = convert2(p,va_arg(args,u64_t),4);
					break;
				case 'd': //10进制,64bits(这里具体要看addr_t的位宽了)
					p = convertr(p,(addr_t)va_arg(args,u64_t),10);
					break;
				case 'o': //8进制,64bits
					p = convert2(p,va_arg(args,u64_t),3);
					break;
				case 'b': //2进制,64bits
					p = convert2(p,va_arg(args,u64_t),1);
					break;
				default:
					//格式错误
					break;
			}
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
	cpuflag_t cpuflag;
	/**
	 * 显存需要互斥访问，避免写冲突
	 */
	save_flags_cli(&cpuflag);
	
	char_t buffer[BUFFER_SIZE];		//字符缓冲区
	va_list ap;
	va_start(ap,fmt);
	_sprintk(buffer,fmt,ap);			//调用sprintk，进行字符串格式化
	va_end(ap);
	
	write_string((const char_t*)buffer);		//将字符串输出到屏幕上
	
	restore_flags_sti(&cpuflag);
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


//==============================================================================================
void stdio_test_main(){
	
	
	
	die(0);
	return;
}
