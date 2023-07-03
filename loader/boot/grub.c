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
    
	image_display((addr_t)IMAGE_ADR);
	die(0x200);

    	//初始化位示图，进行简单内存管理
    	memset(BITMAP_ADR,0,0x1000);	//清空指定区域内存
    	allocate_init(BITMAP_ADR,0x100000,0x8000000);
    	
    	printk("===============================loading loader.bin=============================\n");
    	load_file(LOADER_NAME,LOADER_ADR);
    	die(0x200);
    	
    	printk("===============================loading bios.bin===============================\n");
    	load_file(BIOS_NAME,BIOS_ADR);
    	die(0x200);
    	
    	printk("====================================end...====================================\n");
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
    	if(memcpy(src,size,dst)==0){
    		printk("error in loading the <%s>...\n",fname);       
    	}
    
    	//设置这片内存区域为保留
    	reserve(dst,size);
    	return;
}