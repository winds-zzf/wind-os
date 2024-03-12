/**
 * 文件系统
 */
#include "globaltype.h"
#include "globalctrl.h"

static void inode_t_init(inode_t* inode){
	return;
}

/**
 * 初始化超级块
 */
static void superblock_t_init(superblock_t* super){
	spinlock_init(&super->lock);
	super->flag = 0;
	//初始化位示图
	memset(super->bitmap,0,BITMAP_MAX_SIZE);
	super->bitmap[0] = 1;	//预留0
		
	//初始化根目录
	memset(super->root,0,sizeof(super->root));
	return;
}

/**
 * 初始化文件系统结构
 */
static void devext_t_init(devext_t* ext){
	spinlock_init(&ext->lock);
	list_t_init(&ext->hook);
	ext->status = 0;
	ext->flag = 0;
	ext->start = NULL;
	ext->size = 0;
	ext->ext = NULL;
	
	return;
}

/**
 * 分配用于模拟磁盘
 */
drvstus_t new_devext(device_t* devp){
	//分配模拟存储介质的内存空间4MB
	addr_t disk = mempool_new(0x400000);
	if(NULL==disk){
		return DFCERRSTUS;
	}
	//分配devext实例的存储空间
	devext_t *ext = (devext_t*)mempool_new(sizeof(devext_t*));
	
	//初始化devext结构
	devext_t_init(ext);
	ext->start = (void*)disk;
	
	//将ext作为device_t的拓展数据
	devp->extdata = ext;
	
	return DFCOKSTUS;	
}


/**
 * 申请一个4KB大小的缓冲区，并清空
 */
static void* buf_new(){
	void* buf = (void*)mempool_new(0x1000);
	if(NULL==buf){
		return NULL;
	}
	memset(buf,0,0x1000);
	return buf;
}

/**
 * 释放一个4KB大小的缓冲区
 */
static bool_t buf_delete(void* buf){
	return mempool_delete((addr_t)buf,0x1000);
}


/**
 * 读取块
 */
static drvstus_t block_read(device_t* devp,void *buf,addr_t idx){
	//从设备拓展字段中获取存储设备地址
	devext_t* devext = (devext_t*)devp->extdata;
	
	//将逻辑块地址转换为物理块地址
	addr_t adr = idx*0x1000+(addr_t)devext->start;	//物理存储设备起始地址

	//将buf对应的块拷贝到adr处
	memcpy(buf,(void*)adr,0x1000);
	
	return DFCOKSTUS;
}

/**
 * 写入块
 */
static drvstus_t block_write(device_t* devp,void* buf,addr_t idx){
	//从设备拓展字段中获取存储设备地址
	devext_t* devext = (devext_t*)devp->extdata;
	
	//将逻辑块地址转换为物理块地址
	addr_t adr = idx*0x1000+(addr_t)devext->start;	//物理存储设备起始地址

	//将buf对应的块拷贝到adr处
	memcpy((void*)adr,buf,0x1000);
	
	return DFCOKSTUS;
}


/**
 * 读取超级块
 */
static superblock_t* superblock_read(device_t* devp){
	//获取一个4KB的缓冲区并清零
	void* buf = buf_new();

	//将超级块读取到缓冲区
	block_read(devp,buf,0);
	return (superblock_t*)buf;
}

/**
 * 写回超级块
 */
static void superblock_write(device_t* devp,superblock_t* super){
	//写回超级块
	block_write(devp,(void*)super,0);
	
	//释放缓冲区
	buf_delete(super);
	return;
}

/**
 * 创建超级块
 */
static bool_t superblock_create(device_t* devp){
	//分配一个4KB大小的缓冲区并清零
	void* buf = buf_new();
	
	//在缓冲区开始处初始化一个超级块
	superblock_t_init((superblock_t*)buf);
	
	//把缓冲区中超级块数据写入存储设备的第0块
	if(block_write(devp,buf,0)==DFCERRSTUS){
		return FALSE;
	}
	
	//释放缓冲区
	buf_delete(buf);
	return TRUE;
}

/**
 * 分配空闲块
 * return：分配的空闲块号
 */
static u64_t block_apply(superblock_t* super){
	for(u64_t i=0;i<1024;i++){
		if(0==super->bitmap[i]){
			super->bitmap[i] = 1;
			return i;
		}
	}
	return 0;
}

/**
 * 释放空闲块
 * idx: 待释放的空闲块号
 */
static bool_t block_free(superblock_t* super,u64_t idx){
	//越界的块
	if(idx>1024){
		return FALSE;
	}
	//重复释放
	if(1 != super->bitmap[idx]){
		return FALSE;
	}

	//复位
	super->bitmap[idx] = 0;
	return TRUE;
}


/**
 * 格式化文件系统
 */
static void rfc_format(device_t* devp){
	if(superblock_create(devp)==FALSE){
		printk("sysrfc_format error");
		die(0);
	}
	return;
}


static void rfc_init(device_t* devp){
	rfc_format(devp);
	return;
}

/**
 * 文件系统基本操作
 */

/**
 * 创建文件
 */
drvstus_t rfs_new_file(device_t* devp,char_t* fname){	
	drvstus_t rets = DFCERRSTUS;
	superblock_t* super = superblock_read(devp);
	if(NULL==super){
		return DFCERRSTUS;
	}
	//查找是否有同名文件
	sint_t empty = -1;
	for(uint_t i=0;i<CATALOG_MAX_NUM;i++){
		if(0==super->root[i].fname[0]){
			empty = i;	//记录这个空闲位置
		} else if(strcmp(super->root[i].fname,fname)==TRUE){
			//存在重名文件
			rets = DFCERRSTUS;			
			goto out;
		}
	}

	//检查是否还有空闲位置
	if(empty<0){
		rets = DFCERRSTUS;
		goto out;
	}

	//将新文件名填入文件目录
	strcpy(super->root[empty].fname,fname);
	inode_t_init(&super->root[empty].inode);	//初始化索引节点
	super->root[empty].inode.curblock = block_apply(super);
	rets = DFCOKSTUS;
	
out:
	superblock_write(devp,super);
	return rets;
}
/**
 * 删除文件
 */
drvstus_t rfs_delete_file(device_t* devp,char_t* fname){
	drvstus_t rets = DFCERRSTUS;

	superblock_t* super = superblock_read(devp);
	if(NULL==super){
		return DFCERRSTUS;
	}

	uint_t idx;

	for(idx = 0;idx<CATALOG_MAX_NUM;idx++){
		printk("%s\n",super->root[idx].fname);
		if(0==super->root[idx].fname[0]) continue;		//查找下一项
		if(strcmp(super->root[idx].fname,fname)==0) break;		//查好成功
	}

	//检查待删除文件是否存在
	if(idx>=CATALOG_MAX_NUM){
		rets = DFCERRSTUS;
		goto out;
	}

	//清空文件名
	super->root[idx].fname[0] = 0;
	inode_t* inode = &super->root[idx].inode;
	//释放文件逻辑地址空间对应的物理块
	block_free(super,inode->curblock);

	rets = DFCOKSTUS;
out:

	superblock_write(devp,super);
	return rets;
}


/**
 * 打开文件
 */
drvstus_t rfs_open_file(device_t* devp,void* iopack){
	objnode_t* obj = (objnode_t*)iopack;
	//检查文件名是否为空
	if(NULL==obj->fname){
		return DFCERRSTUS;
	}
	
	//尝试打开文件
	superblock_t* super = superblock_read(devp);

	if(NULL==super){		
		return DFCERRSTUS;
	}

	//查找文件对应索引接点
	for(uint_t i = 0;i<CATALOG_MAX_NUM;i++){
		if(0==super->root[i].fname[0]) continue;		//查找下一项
		if(strcmp(super->root[i].fname,obj->fname)==0){
			//查找成功
			obj->fnode = ((devext_t*)devp->extdata)->start+((addr_t)&super->root[i].inode-(addr_t)super);
			superblock_write(devp,super);
			return DFCOKSTUS;
		}
	}

	//查找失败
	superblock_write(devp,super);
	return DFCERRSTUS;
}
/**
 * 关闭文件
 */
drvstus_t rfs_close_file(device_t* devp,void* iopack){
	objnode_t* obj = (objnode_t*)iopack;
	//检查文件是否已经打开了
	if(NULL==obj->fnode){
		return DFCERRSTUS;
	}
	
	//关闭文件
	obj->fnode = NULL;
	
	//释放缓冲区
	return DFCOKSTUS;
}


static drvstus_t fileblock_read(device_t* devp,inode_t* inode,void* ubuf,size_t len){
	//不支持超过4KB的文件读写
	if(len>4096){
		return DFCERRSTUS;
	}

	//分配一个内核块缓冲区
	void* kbuf = buf_new();
	
	//从硬盘中读取指定块到内核缓冲区
	block_read(devp,kbuf,inode->curblock);
	
	//从内核缓冲区中读取len数据到用户缓冲区
	memcpy(ubuf,(void*)((addr_t)kbuf+inode->curoff),len);

	//更新位置指针
	inode->curoff+=len;
	
	//释放内核块缓冲区
	buf_delete(kbuf);
	return DFCOKSTUS;
}

/**
 * 读文件
 */
drvstus_t rfs_read_file(device_t* devp,void* iopack){
	objnode_t* obj = (objnode_t*)iopack;
	//检查索引节点是否为空，检查文件缓冲区是否合法
	if(NULL==obj->fnode || NULL==obj->buf || 0x1000 != obj->bufsz){
		return DFCERRSTUS;
	}
	//从文件当前位置读取len个字节到缓冲区
	return fileblock_read(devp,obj->fnode,obj->buf,obj->len);
}


static drvstus_t fileblock_write(device_t* devp,inode_t* inode,void* ubuf,size_t len){
	//不支持超过4KB的文件读写
	if(len>4096){
		return DFCERRSTUS;
	}

	//分配一个内核块缓冲区
	void* kbuf = buf_new();
	
	//从硬盘中读取指定块到内核缓冲区
	block_read(devp,kbuf,inode->curblock);

	//从用户缓冲区读取len大小到内核缓冲区
	memcpy((void*)((addr_t)kbuf+inode->curoff),ubuf,len);

	//更新位置指针
	inode->curoff+=len;

	//将修改后的块写回内存
	block_write(devp,kbuf,inode->curblock);
	
	//释放内核块缓冲区
	buf_delete(kbuf);
	return DFCOKSTUS;
}

/**
 * 写文件
 */
drvstus_t rfs_write_file(device_t* devp,void* iopack){
	objnode_t* obj = (objnode_t*)iopack;
	//检查索引节点是否为空，检查文件缓冲区是否合法
	if(NULL==obj->fnode || NULL==obj->buf || 0x1000 != obj->bufsz){
		return DFCERRSTUS;
	}
	return fileblock_write(devp,obj->fnode,obj->buf,obj->len);
}

/**
 * 定位指针
 */

/**
 * 新建目录
 */

/**
 * 删除目录
 */

/**
 * 复制文件
 */
 
/**
 * 复制目录 
 */

/**
 * 文件查找
 */

/**
 * 遍历目录
 */

/**
 * 内存映射文件
 */


drvstus_t rfs_open(device_t *devp,void* iopack){
	objnode_t* obj = (objnode_t*)iopack;
	//根据访问标志判断是否新建文件
	if(obj->acsflags == OBJNODE_ACCESSFLAG_OPEN){
		return rfs_open_file(devp,iopack);
	}
	else if(obj->acsflags == OBJNODE_ACCESSFLAG_NEW){
		return rfs_new_file(devp,obj->fname);		
	}
	return DFCERRSTUS;
}

drvstus_t rfs_close(device_t* devp,void* iopack){
	return rfs_close_file(devp,iopack);
}

drvstus_t rfs_read(device_t* devp,void* iopack){	
	return rfs_read_file(devp,iopack);
}

drvstus_t rfs_write(device_t* devp,void* iopack){
	return rfs_write_file(devp,iopack);
}

drvstus_t rfs_lseek(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t rfs_ioctrl(device_t* devp,void* iopack){
	objnode_t* obj = (objnode_t*)iopack;
	printk("sysrfs_ioctrl:0\n");	
	if(obj->ioctrl==OBJNODE_IOCTRL_DELETE){
		printk("sysrfs_ioctrl:1\n");
		return rfs_delete_file(devp,obj->fname);
	}
	return DFCERRSTUS;
}

drvstus_t rfs_dev_start(device_t* devp,void* iopack){
	
	return DFCERRSTUS;
}

drvstus_t rfs_dev_stop(device_t* devp,void* iopack){
	
	return DFCERRSTUS;
}

drvstus_t rfs_set_powerstus(device_t* devp,void* iopack){
	
	return DFCERRSTUS;
}

drvstus_t rfs_enum_dev(device_t* devp,void* iopack){
	
	return DFCERRSTUS;
}

drvstus_t rfs_flush(device_t* devp,void* iopack){
	
	return DFCERRSTUS;
}

drvstus_t rfs_shutdown(device_t* devp,void* iopack){
	
	return DFCERRSTUS;
}

drvstus_t rfs_handle(uint_t intnum,device_t* devp,void* sframe){
	return DFCERRSTUS;
}


/**
 * 配置设备描述符
 */
static void set_device(device_t* devp,driver_t* drvp){
	//设置设备的抽象类型
	devp->id.mtype = FILESYS_DEVICE;
	devp->id.stype = 0;
	devp->id.num = 0;	//设备编号
	//设备名称
	devp->name = "sysrfs";
	return;
}

/**
 * 配置驱动描述符
 */
static void set_driver(driver_t* drvp){
	//设置驱动程序功能派发函数
	drvp->dipfun[IOIF_CODE_OPEN] = rfs_open;
	drvp->dipfun[IOIF_CODE_CLOSE] = rfs_close;
	drvp->dipfun[IOIF_CODE_READ] = rfs_read;
	drvp->dipfun[IOIF_CODE_WRITE] = rfs_write;
	drvp->dipfun[IOIF_CODE_LSEEK] = rfs_lseek;
	drvp->dipfun[IOIF_CODE_IOCTRL] = rfs_ioctrl;
	drvp->dipfun[IOIF_CODE_DEV_START] = rfs_dev_start;
	drvp->dipfun[IOIF_CODE_DEV_STOP] = rfs_dev_stop;
	drvp->dipfun[IOIF_CODE_SET_POWERSTUS] = rfs_set_powerstus;
	drvp->dipfun[IOIF_CODE_ENUM_DEV] = rfs_enum_dev;
	drvp->dipfun[IOIF_CODE_FLUSH] = rfs_flush;
	drvp->dipfun[IOIF_CODE_SHUTDOWN] = rfs_shutdown;
	drvp->name = "systick0drv";	//设置驱动程序名称
	return;
}

void drvrfc_test_main(device_t* devp);

drvstus_t drvrfs_entry(driver_t *drvp,u64_t val,void* p){
	//分配一个设备描述符
	device_t* devp = device_new();
	//配置驱动程序和设备信息
	set_driver(drvp);
	set_device(devp,drvp);

	//分配模拟存储设备的内存空间
	if(new_devext(devp)==DFCERRSTUS){
		return DFCERRSTUS;
	}	

	//将设备加入到驱动程序
	if(device_add_driver(devp,drvp)==DFCERRSTUS){
		return DFCERRSTUS;		
	}

	//向内核注册设备
	if(device_register(devp)==DFCERRSTUS){
		return DFCERRSTUS;
	}

	/**
	 * 为什么不用安装中断回调函数
	 */
	 
	//初始化设备
	rfc_init(devp);

	//drvrfc_test_main(devp);
	return DFCOKSTUS;
}

drvstus_t drvrfs_exit(driver_t *drvp,u64_t val,void* p){
	
	return DFCERRSTUS;
}



//============================================================================================
/**
 * 
 */
INLINE void test_superblock(device_t* devp){
	printk("test file system super block\n");
	superblock_t *super = superblock_read(devp);
	for(uint_t i=0;i<BITMAP_MAX_SIZE;i++){
		printk("%d",super->bitmap[i]);
	}
	for(uint_t i=0;i<CATALOG_MAX_NUM;i++){
		printk("%s,",super->root[i].fname);
	}
	superblock_write(devp,super);
	return;
}

void drvrfc_test_main(device_t* devp){
	//test_superblock(devp);
	
	//创建并分配数据对象
	objnode_t* obj = (objnode_t*)mempool_new(sizeof(objnode_t));

	obj->acsflags = OBJNODE_ACCESSFLAG_NEW;	//新建文件状态
	obj->fname = "main.exe";					//文件名
	obj->buf = buf_new();					//文件对应的用户缓冲区
	obj->bufsz = 0x1000;					//用户缓冲区大小
	obj->len = 512;						//每次读写大小
	obj->ioctrl = OBJNODE_IOCTRL_DELETE;		//设置控制码
	//尝试新建文件							//
	if(rfs_open(devp,obj)==DFCERRSTUS){
		printk("create file failure!!!\n");die(0);
	}
	
	superblock_t *super = superblock_read(devp);
	for(uint_t i=0;i<BITMAP_MAX_SIZE;i++){
		printk("%d",super->bitmap[i]);
	}
	for(uint_t i=0;i<CATALOG_MAX_NUM;i++){
		printk("%s,",super->root[i].fname);
	}
	printk("\n");
	superblock_write(devp,super);
	
	
	//尝试打开文件
	obj->acsflags = OBJNODE_ACCESSFLAG_OPEN;	//新建文件状态
	if(rfs_open(devp,obj)==DFCERRSTUS){
		printk("open file failure!!!\n");die(0);
	}

	//尝试写入文件
	memset(obj->buf,0xff,512);
	if(rfs_write(devp,obj)==DFCERRSTUS){
		printk("write file failure!!!\n");die(0);
	}
	
	//尝试读取文件
	((inode_t*)(obj->fnode))->curoff = 0;
	memset(obj->buf,0,512);
	if(rfs_read(devp,obj)==DFCERRSTUS){
		printk("read file failure!!!\n");die(0);
	}
	u8_t* p = (u8_t*)obj->buf;
	for(uint_t i=0; i<512; i++){
		if(p[i]!=0xff){
			printk("write/read file failure!!!\n");die(0);
		}
	}
	//尝试关闭文件
	if(rfs_close(devp,obj)==DFCERRSTUS){
		printk("close file failure!!!\n");die(0);
	}	
	printk("0x%lx\n",obj->fnode);
	printk("%s\n",obj->fname);

	//尝试删除文件	
	if(rfs_ioctrl(devp,obj)==DFCERRSTUS){
		printk("delete file failure!!!\n");die(0);
	}		
	super = superblock_read(devp);
	for(uint_t i=0;i<BITMAP_MAX_SIZE;i++){
		printk("%d",super->bitmap[i]);
	}
	for(uint_t i=0;i<CATALOG_MAX_NUM;i++){
		printk("%s,",super->root[i].fname);
	}
	printk("\n");
	superblock_write(devp,super);

	
	printk("successfully pass the sysrfs_test_main test\n");
	die(0);
	return;
}
