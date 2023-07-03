/**
 * 文件输入输出控制
 */

#include<stdio.h>
#include<stdlib.h>
#include"global.h"
#include"io.h"

 /**
  * 将内存数据拷贝到内存中
  */
Status mem_to_file(void* mem,u32_t size,FILE* file, u32_t offset){
	if (NULL == mem || NULL == file) {		//参数检查
		printf("error:@%s\n",__FUNCTION__);
		return ERROR;
	}
	fseek(file,offset,SEEK_SET);	//将文件指针设置为指定偏移
	fwrite(mem,size,1,file);		//将内存数据以二进制的形式写入文件
	return OK;
}

/**
 * 将文件内容拷贝到指定地址下
 */
Status mem_from_file(void* mem,u32_t size,FILE* file, u32_t offset) {
	if (NULL == mem || NULL == file) {		//参数检查
		printf("error:@%s\n", __FUNCTION__);
		return ERROR;
	}
	fseek(file,offset,SEEK_SET);
	fread(mem,size,1,file);
	return OK;
}

/**
 * 文件拷贝
 * 将一个文件拷贝到另一个文件的偏移处
 */
u32_t file_copy(FILE* src,FILE* dst,u32_t offset) {	
	fseek(dst,offset,SEEK_SET);
	return file_append(src,dst);
}

/**
 * 将一个文件中的部分数据拷贝到量一个文件中
 */
Status copy_file(FILE*src,FILE* dst,u32_t offset,u32_t size) {
	char buffer[1024];
	u32_t count = 1024;
	fseek(src, offset,SEEK_SET);	//设置源文件的偏移量
	while (size>0) {
		count = size>1024 ? 1024:size;	//确定本次要读取的字节数
		count = fread(buffer, 1, count, src);	//count以实际读取的为准
		fwrite(buffer, 1, count, dst);
		size -= count;
	}
	return OK;
}

/**
 * 追加文件
 */
u32_t file_append(FILE* src, FILE* dst) {
	char buffer[1024];	//缓冲区
	u32_t sum = 0;
	u32_t count;
	while ((count = fread(buffer, 1, 1024, src)) > 0) {
		if (ferror(src)) {
			printf("error:read error @%s",__FUNCTION__);
		}
		fwrite(buffer, 1, count, dst);
		sum += count;
	}
	return sum;
}

/**
 * 
 */
FILE* get_fp(const char* file,const char* mode) {
	FILE* fp;
	if (NULL == file || 0 == file[0]) {	//参数检查
		printf("error:parameters is null or empty @io.%s\n",__FUNCTION__);
		exit(1);
	}
	if ((fp = fopen(file, mode)) == NULL) {
		printf("error:failed to open the file <%s> @io.%s\n",file,__FUNCTION__);
		exit(1);
	}
	return fp;
}