/**
 * 内存映像文件处理
 * API实现
 */
 
/**
 * 注意：内存映像文件解析模块是不知道内存中映像文件位置的，另外需要注意映像文件中保存的全部是相对路径
 */

#include"lib.h"
#include"image_t.h"

/**
 * 验证文件校验和
 * addr:内存地址
 * size:内存区域大小
 * checksum：源校验和
 */
static boolean_t verify(addr_t addr,size_t size,u32_t checksum){
	return checksum==checksum_crc32(addr,size,CRC32_POLY)? TRUE:FALSE;
}

void image_display(const addr_t adr){
	printk("=================================image info==================================\n");
	Header *header = (Header*)(adr+HEADER_OFFSET);		//获取映像头地址
	Handle *fhds= (Handle*)((addr_t)header+sizeof(Header));	//文件头数组地址
	//输出映像头信息
	printk("version:\t%d\n",header->version);
	printk("start:\t0x%x\n",header->start);
	printk("size:\t%d\n",header->size);
	printk("checksum:\t0x%x\n",header->checksum);
	//文件头校验和
	if (!verify((addr_t)fhds,sizeof(Handle)*header->size,header->checksum)) {
		printk("the kernel image file handles is damaged!\n");
		return;
	}
	//输出文件头数组信息
	printk("fname\t\tstart\t\tsize\t\tchecksum\n");
	for (u32_t i = 1; i < header->size; i++) {	//grub.bin文件加载后就在内存中被修改了，因此校验和没有实用价值
		printk("%s\t\t",fhds[i].fname);
		printk("0x%x\t\t",fhds[i].start);
		printk("%d\t\t",fhds[i].size);
		printk("0x%x\n",fhds[i].checksum);
		//文件校验和
		if (!verify(adr+fhds[i].start,fhds[i].size,fhds[i].checksum)) {
			printk("the target file <%s> in kernel image is damged!\n",fhds[i].fname);
		}
	}
	printk("=================================image end.====================================\n");
	return;
}

Handle* get_file(const addr_t adr,const char_t *fname){					
	Header *header = (Header*)(adr+HEADER_OFFSET);		//获取映像头地址
	Handle *fhds= (Handle*)((addr_t)header+sizeof(Header));	//文件头数组地址
	
	if (!verify((addr_t)fhds,sizeof(Handle)*header->size,header->checksum)) {
		printk("the kernel image file handles is damaged!\n");
	}
	
	for(u32_t i=0;i<header->size;i++){		//查找指定文件
		if(strcmp(fhds[i].fname,fname)==0){
			if(verify(adr+fhds[i].start,fhds[i].size,fhds[i].checksum)){	//校验文件
				return fhds+i;
			}else{
				printk("the target file <%s> in kernel image is damged!\n",fhds[i].fname);
			}
		}
	}
	return NULL;	//查找失败
}

size_t get_image_file(const addr_t image_adr,const char_t* fname,addr_t* adr){
	Handle *fhd = get_file(image_adr,fname);	//获取文件句柄
	
	if(NULL==fhd) return 0;	//获取失败
	*adr = fhd->start+image_adr;

	return fhd->size;		
}
