/**
 * 加载内核映像文件中的二级引导器，和BIOS调用模块
 */

#include"global.h"

static void load_file(const char_t* fname,const addr_t dst);

/**
 * 加载映像文件
 */
void grub_entry(){
	init_cursor();
	clear_screen(0x0f00);
	printk("==================================grub start==================================\n");

    	//初始化内存位示图
    	memset(BITMAP_ADR,0,0x1000);	//清空指定区域内存
    	allocate_init(BITMAP_ADR,0x100000,0x8000000);
	
	//输出内核映像信息
	image_display((addr_t)IMAGE_ADR);
    	
    	printk("loading loader.bin...\n");
    	load_file(LOADER_NAME,LOADER_ADR);
    	
    	printk("loading bios.bin...\n");
    	load_file(BIOS_NAME,BIOS_ADR);
    	
    	printk("==================================grub end.===================================\n");
    	die(0x200);
    	return;
}

/**
 * 赋值指定文件到内存中
 * fname:映像中的文件名
 * dst:加载的目标地址
 */
static void load_file(const char_t* fname,const addr_t dst){
    	addr_t src = NULL;		//文件地址
    	size_t size = 0;		//文件大小
    	size = get_image_file(IMAGE_ADR,fname,&src);
    	
    	//设置这片内存区域为保留
    	reserve(dst,size);
    	
    	if(memcpy(src,size,dst)==0){
    		printk("error in loading the <%s>...\n",fname);     
    	}
    	
    	return ;
}