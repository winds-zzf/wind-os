/**
 * 内核映像处理
 * API定义
 */
#ifndef __IMAGE_H
#define __IMAGE_H


#include "image_t.h"

/**
 * 展示映像文件信息
 * addr:内核映像物理地址
 */
void image_display(const addr_t addr);

/**
 * 获取映像文件中指定文件的文件句柄
 * addr:内核映像物理地址
 * fname:文件名
 */
FHandle* get_file(const addr_t addr,const char_t *fname);

/**
 * 获取之规定文件句柄
 * image_adr:内核映像地址
 * fname:文件名
 * adr:文件地址
 */
size_t get_image_file(const addr_t image_adr,const char_t* fname,addr_t* adr);

/**
 * 获取image文件大小
 * adr: 映像文件地址
 */
size_t get_image_size(addr_t adr);

#endif //__IMAGE_H