/**
 * 映像打包解包工具调用接口
 */

#ifndef __PACK_H
#define __PACK_H


/*
* 打包函数
*/
Status pack(const char* fname,char fnames[][FNAME_MAX_LEN], int len);

/*
* 解包函数
*/
Status unpack(const char* filename);


#endif