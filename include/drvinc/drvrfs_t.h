/**
 * 文件系统
 * API定义
 */
#ifndef __DRVRFS_T_H
#define __DRVRFS_T_H


#define CATALOG_MAX_NUM 20
#define BITMAP_MAX_SIZE 1024


/**
 * 文件管理头
 */
typedef struct INODE_T{
	u64_t	status;		//文件状态
	u64_t	flag;		//文件标志

	u64_t	start;		//文件起始块号
	u64_t	num;			//文件总块数
	u64_t	size;		//文件实际大小
	
	u64_t	curblock;		//文件当前将要写入的逻辑存储块
	u64_t	curoff;		//文件在当前逻辑存储块中的偏移
}inode_t;


/* 文件超级块 */
typedef struct SUPERBLOCK_T{
	spinlock_t	lock;		//保护该结构的自旋锁 
	u64_t	flag;		//标志
	
	//位示图
	u8_t		bitmap[BITMAP_MAX_SIZE];	//位图的开始逻辑块4MB:1024*4KB
	
	struct {
		char_t fname[10];	//按名索引
		inode_t inode;		//索引接点
	}root[CATALOG_MAX_NUM];
}superblock_t;


/* 设备拓展数据 */
typedef struct DEVEXT_T{
	spinlock_t	lock;		//保护该结构的自旋锁
	list_t		hook;		//不知道要干嘛用的钩子
	u64_t	status;		//状态
	u64_t	flag;		//标志
	void*	start;		//开始地址
	size_t	size;		//大小
	void*	ext;			//拓展用
}devext_t;


#endif //__DRVRFS_T_H