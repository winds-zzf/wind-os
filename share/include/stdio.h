/**
 * 字符串处理
 * API定义
 */
#ifndef __STDIO_H
#define __STDIO_H

/**
 * 缓冲区格式化输入函数
 * fmt:格式控制字符串
 * ...:参数列表
 */
void sscank(char_t* buffer, const char_t* fmt,...);

/**
 * 控制台格式化输入函数
 * fmt:格式控制字符串
 * ...:参数列表
 */
void scank(const char_t* fmt,...);

/**
 * 缓冲区格式化输出操作,参考sprintf()
 * buffer:字符串缓冲区
 * fmt:格式化控制字符
 * ...:参数列表
 */
void sprintk(char_t* buffer,const char_t* fmt,...);

/**
 * 控制台格式化输出函数
 * fmt:格式化控制字符
 * ...:参数列表
 */
void printk(const char_t* fmt,...);

#endif //__STRING_H