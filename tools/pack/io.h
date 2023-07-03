/**
 * 文件输入输出控制接口
 */

#ifndef __IO_H
#define __IO_H
#include "stdio.h"

 /**
  * 将内存数据拷贝到内存中
  */
Status mem_to_file(void* mem, u32_t size, FILE* file, u32_t offset);

/**
 * 将文件内容拷贝到指定地址下
 */
Status mem_from_file(void* mem, u32_t size, FILE* file, u32_t offset);

/**
 * 文件拷贝
 * 将一个文件拷贝到另一个文件的偏移处
 */
u32_t file_copy(FILE* src, FILE* dst, u32_t offset);

Status copy_file(FILE* src, FILE* dst, u32_t offset, u32_t size);

/**
 * 
 */
u32_t file_append(FILE* src, FILE* dst);

/**
 * 
 */
FILE* get_fp(const char* file,const char* mode);

#endif