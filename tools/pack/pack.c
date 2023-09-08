/**
 * 映像打包解包工具函数实现
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"global.h"
#include"image_t.h"
#include"pack.h"
#include"io.h"

#define CRC32_POLY 0xC96C5795U	//CRC32多项式
#define CRC32_INIT 0xFFFFFFFFU	//CRC32初始值

/**
 * 打印num的16进制形式
 * num:输入整数
 */
static void print(u32_t num,u32_t base) {
	char buffer[33];
	int i=33;
	buffer[i] = 0;
	while (num>0&&i>=0) {
		i--;
		buffer[i] = "0123456789ABCDEFG"[num%base];
		num /= base;
	}
	printf("%s\n",buffer+i);
}

/**
 * 计算内存数据的校验和
 */
u32_t checksum_mem(void* addr, u32_t size) {
	u32_t crc = CRC32_INIT;
	unsigned char* data = (unsigned char*)addr;
	for (u32_t i = 0; i < size; i++) {
		crc ^= data[i];
		for (int j = 0; j < 8; j++) {
			if (crc & 1)
				crc = (crc >> 1) ^ CRC32_POLY;
			else
				crc >>= 1;
		}
	}
	crc ^= CRC32_INIT;
	return crc;
}

/**
 * 计算文件校验和
 */
u32_t checksum_file(FILE* file) {
	u32_t crc = CRC32_INIT;
	fflush(file);
	rewind(file);
	int data;		//data应该使用int类型，因为fgetc()返回值是int，类型一致避免类型转化或者数据截断
	while ((data=fgetc(file))!=EOF) {
		crc ^= data&0xff;	//取出低8位(使用&提取特定字节，比使用类型转化具有更好的安全性)
		for (int j = 0; j < 8; j++) {
			if (crc & 1)
				crc = (crc >> 1) ^ CRC32_POLY;
			else
				crc >>= 1;
		}
	}
	crc ^= CRC32_INIT;
	return crc;
}

/**
 * 初始化文件头
 */
static void init_header(Header* header) {
	header->version = 0;
	header->start = 0;
	header->size = 0;
	header->checksum = 0;
}

/**
 * 初始化文件头
 */
static void init_handle(Handle* handle) {
	memset(handle->fname,0,sizeof(handle->fname));
	handle->start = 0;
	handle->size = 0;
	handle->checksum = 0;
}

/*
 * 打包函数
 * 结构体数组传参，如果是二维数组，则应该给形参指明列长，不然形参指针和实参指针结构会不一致
 */
Status pack(const char* fname,char fnames[][FNAME_MAX_LEN], int len) {
	//映像头
	Header header;
	//文件头数组
	Handle* fhds = (Handle*)malloc(len * sizeof(Handle));
	//文件句柄分配
	FILE* image = get_fp(fname,"wb+");	//映像文件

	//映像头和文件头从0x4000=16KB处连续存放
	init_header(&header);
	header.version = VERSION;
	header.start = 0x4000 + sizeof(Header);	//文件头起始地址
	header.size = len;	//文件数目
	
	//文件从0x5000=20KB处连续存放
	for (u32_t i = 0, base=0x0; i < header.size; i++) {
		printf("正在打包%s...\t",fnames[i]);
		FILE* fp = get_fp(fnames[i],"rb+");
		init_handle(&fhds[i]);		//初始化文件头
		strcpy(fhds[i].fname, fnames[i]);
		fhds[i].start = base;
		fhds[i].size = file_copy(fp, image, base);
		fhds[i].checksum = checksum_file(fp);
		base += (i == 0)? 0x5000 : fhds[i].size;
		fclose(fp);
		printf("打包完成:0x%X\t%d\t0x%X\n",fhds[i].start,fhds[i].size,fhds[i].checksum);
	}
	header.checksum = checksum_mem(fhds,sizeof(Handle)*header.size);	//计算文件头结构体数组的校验和
	
	//写入映像头
	mem_to_file(&header, sizeof(header), image, 0x4000);
	
	//写入文件头
	mem_to_file(fhds,sizeof(Handle)* header.size,image,0x4000+sizeof(Header));

	//文件句柄和动态内存释放
	fclose(image);
	free(fhds);
	
	return OK;
}

/*
* 解包函数
*/
Status unpack(const char* fname) {
	FILE* img = get_fp(fname, "rb+");	//映像文件
	Header header;	//映像头
	Handle* fhds=NULL;	//文件头数组

	mem_from_file(&header, sizeof(Header),img,0x4000);	//0x4000处读取映像头
	fhds = (Handle*)malloc(sizeof(Handle)*header.size);
	mem_from_file(fhds, sizeof(Handle) * header.size,img,0x4000+sizeof(Header));	//在Header后读取Handle数组
	if (checksum_mem(fhds,sizeof(Handle)*header.size)!=header.checksum) {	//CRC校验和0不为0
		printf("fhds is damaged!");
		return ERROR;
	}
	for (u32_t i = 0; i < header.size; i++) {
		printf("%s\t%d\t%d\t", fhds[i].fname,fhds[i].start,fhds[i].size);
		print(fhds[i].checksum,16);
		FILE* fp = get_fp(fhds[i].fname, "wb+");
		copy_file(img,fp,fhds[i].start,fhds[i].size);
		if (checksum_file(fp)!=fhds[i].checksum) {
			printf("%s is damaged!", fhds[i].fname);
		}

		fclose(fp);fp = NULL;
	}

	fclose(img);
	free(fhds);
	return OK;
}