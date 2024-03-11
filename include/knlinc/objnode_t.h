/**
 * 对象节点
 * 类型定义
 */
#ifndef __OBJNODE_T_H
#define __OBJNODE_T_H

#define OBJNODE_ACCESSFLAG_OPEN 0
#define OBJNODE_ACCESSFLAG_NEW 1
#define OBJNODE_IOCTRL_DELETE 2
/* 资源描述符 */
typedef struct OBJNODE_T{
	spinlock_t		lock;		//保护objnode的自旋锁
	list_t			hook;		//钩子
	sem_t		sem;			//完成信号量
	u64_t		flags;		//标志
	u64_t		status;		//状态
	
	s64_t		opcode;		//操作码
	u64_t		objtype;		//对象类型(文件或设备)
	u64_t		objadr;		//对象地址
	
	u64_t		acsflags;		//访问设备/文件标志
	u64_t		acsstus;		//访问设备/文件状态
	
	u64_t		curloc;		//对应于读写数据的当前位置	
	u64_t		len;			//对应于读写位置的长度
	
	u64_t		ioctrl;		//io控制码

	//资源占用的缓冲区
	buf_t		buf;			//对应于读写数据的缓冲区
	u64_t		bufcurloc;	//对应于读写数据的缓冲区的当前位置
	s64_t		bufsz;		//对应于读写数据的缓冲区的大小
	
	u64_t		count;		//对应于对象节点的计数
	
	void*		safedsc;		//对应于对象节点的安全描述符
	//进程打开文件
	void*		fname;		//打开文件的名称
	void*		fnode;		//打开文件的索引接点
	
	void*		expt;		//用于拓展
}objnode_t;

/* 数据对象类型 */
#define OBJNODE_TYPE_NULL	0	//空类型
#define OBJNODE_TYPE_DEVICE	1	//设备数据对象
#define OBJNODE_TYPE_FILE	2	//文件数据对象


#endif //__OBJNODE_T_H