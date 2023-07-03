/**
 * 映像文件处理模块
 * 类型定义
 */
#ifndef _IMAGE_T_H
#define _IMAGE_T_H

#ifndef u32_t
typedef unsigned int u32_t;
#endif //u32_t

#define GRUB_SIZE 0x4000U			//recognizable GRUB header：16KB
#define HEADER_OFFSET GRUB_SIZE		//映像头偏移地址
#define FILE_OFFSET	0x5000U			//文件偏移地址
#define VERSION 1					//image version
#define FNAME_MAX_LEN 32				//最大文件名长度
#define CRC32_POLY 0xC96C5795U		//CRC32多项式
#define CRC32_INIT 0xFFFFFFFFU		//CRC校验初始值

/**
 * 映像头描述符
 */
typedef struct HEADER{
	u32_t version;		//映像版本
	u32_t start;		//文件头其实偏移
	u32_t size;		//文件头数目
	u32_t checksum;	//文件头校验和
}Header;

/**
 * 文件头描述符
 */
typedef struct HANDLE{
	char fname[FNAME_MAX_LEN];	//文件名
	u32_t start;				//文件起始偏移
	u32_t size;				//文件大小
	u32_t checksum;			//文件校验和
}Handle;


#endif //_IMAGE_T_H