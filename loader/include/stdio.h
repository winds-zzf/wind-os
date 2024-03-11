/**
 * 字符串处理
 * API定义
 */
#ifndef __STDIO_H
#define __STDIO_H


/**
 * 缓冲区格式化输出操作,参考sprintf()
 * buffer: 输入字符缓冲区
 * fmt: 格式化控制字符
 * ...: 参数列表
 */
void sprintk(char_t* buffer,const char_t* fmt,...);

/**
 * 控制台格式化输出函数,参考printf()
 * fmt: 格式化控制字符
 * ...: 参数列表
 */
void printk(const char_t* fmt,...);

/**
 * 
 */
void die(uint_t delay);


#endif //__STRING_H