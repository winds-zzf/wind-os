/**
 * 文件输入输出控制接口
 */

#ifndef __IO_H
#define __IO_H
#include "stdio.h"

/**
 * 将一片内存数据拷贝到文件的指定偏移处
 * mem: 待拷贝的内存起始地址
 * size: 待拷贝内存数据大小
 * file: 目标文件指针
 * offset: 将数据拷贝到文件的offset偏移地址处
 */
Status mem_to_file(void* mem, u32_t size, FILE* file, u32_t offset);

/**
 * 从文件的特定偏移处拷贝指定大小的数据到内存中
 * mem: 文件拷贝的目标内存地址
 * size: 从文件中拷贝数据的大小
 * file: 待拷贝的源文件
 * offset: 从文件offset处开始拷贝
 */
Status mem_from_file(void* mem, u32_t size, FILE* file, u32_t offset);

/**
 * 将一个文件完整的拷贝到另一个文件的指定偏移处
 * src: 待拷贝的源文件
 * dst: 接收数据的目标文件
 * offset: 源文件将被拷贝目标文件offset偏移处
 */
u32_t file_copy(FILE* src, FILE* dst, u32_t offset);

/**
 * 从一个文件中拷贝处一段数据，存入另一个新文件中
 * src: 提供数据块的源文件
 * dst: 由数据块生成的目标文件
 * offset: 从源文件的offset处开始拷贝
 * size: 拷贝数据块的大小
 */
Status copy_file(FILE* src, FILE* dst, u32_t offset, u32_t size);

/**
 * 将src文件中的内容追加到dst文件。数据源是src文件指针后的所有数据，这些数据从dst文件指针位置开始存储
 * src: 待拷贝文件
 * dst: 接收文件
 */
u32_t file_append(FILE* src, FILE* dst);

/**
 * 获取文件FILE指针，封装获取细节以及错误处理
 * file: 文件名
 * mode: 文件打开模式
 */
FILE* get_fp(const char* file,const char* mode);

#endif