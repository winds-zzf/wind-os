/**
 * 加载内核映像文件中的二级引导器，和BIOS调用模块
 */

#include"global.h"

static void load_file(const char_t* fname,const addr_t dst);
static void grub_init();

/**
 * 加载映像文件
 */
void grub_entry(){
	init_cursor();
	clear_screen(0x0f00);
	printk("==================================grub start==================================\n");

	//performing initialization of grub
	grub_init();
    	
    	printk("==================================grub end.===================================\n");
    	die(0x200);
    	
    	return;
}

/**
 * grub阶段的初始化负责加载loader必要的文件
 */
static void grub_init(){
	//1.输出Wind.kif信息
	image_display((addr_t)IMAGE_ADR);

    	//2.将loader.bin加载到内存中
    	load_file(LOADER_NAME,LOADER_ADR);
    	
    	//3.将bios.bin加载到内存中
    	load_file(BIOS_NAME,BIOS_ADR);

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
    	printk("loading %s...\n",fname);
    	if(memcpy(src,size,dst)==0){
    		printk("error in loading the <%s>...\n",fname);     
    	}
    	
    	return ;
}