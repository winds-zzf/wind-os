/**
 * 内核进程模块
 * API实现
 */

#include "globaltype.h"
#include "globalctrl.h"

/**
 * 
 */
static void context_t_init(context_t *context){
	context->nextrip = 0;
	context->nextrsp = 0;

	//因为只有一个CPU，因此始终用第0号tss
	context->nexttss = &x64tss[0];
	
	return;
}


/**
 * 获取进程id
 * 直接使用进程描述符地址作为进程id，既保证了唯一性，又便于id的分配与释放
 */
static u64_t thread_id(thread_t *thread){
	return (u64_t)thread;
}

/**
 * 
 */
static void thread_t_init(thread_t *thread){
	spinlock_init(&thread->lock);
	list_init(&thread->hook);
	
	thread->status = THREAD_STATUS_NEW;	//进程创建态
	thread->flags = THREAD_FLAG_FREE;		//进程空闲
	thread->cpuid = 0;					//因为目前只有一个CPU，这里为0
	thread->id = thread_id(thread);
	thread->tick = 0;
	thread->privilege = THREAD_PRIVILEGE_USR;	//普通用户进程权限
	thread->priority = THREAD_PRIORITY_MIN;	//最高优先级	
	thread->runmode = 0;
	
	thread->krlstktop = NULL;
	thread->krlstkbase = NULL;
	thread->usrstktop = NULL;
	thread->usrstkbase = NULL;

	//初始化进程内存结构
	/**
	 * 为什么指向默认的地址空间结构？
	 */
	thread->mem = &kvmProcess;
	//初始化上下文结构
	context_t_init(&thread->context);
	//初始化资源描述符指针数组
	for(uint_t i=0;i<THREAD_HAND_MAX;i++){
		thread->objs[i] = NULL;	
	}
	
	return;
}


/**
 * 创建一个进程结构体
 */
thread_t* new_thread(){
	//分配一个thread_t大小的结构体
	thread_t* thread = (thread_t*)mempool_new(sizeof(thread_t));
	if(NULL==thread){
		return NULL;
	}
	//初始化刚分配的thread_t
	thread_t_init(thread);
	
	return thread;
}

/**
 * 
 */
bool_t del_thread(){
	
	return FALSE;
}

/**
 * 初始化内核栈
 * 在进程内核栈中放置一份CPU寄存器数据
 * 进程处于等待调用时，栈中应该已经设置好了寄存器的值，供进程调度时上CPU运行
 */
void krnl_stack_init(thread_t* thread,void* entry, cpuflag_t cpuflags){
	/**
	 * 为什么栈顶要16字节对齐？
	 * 为什么没有用到base地址？
	 */
	thread->krlstktop &= (~0xf);	//低4位清零 
	thread->usrstktop &= (~0xf);
	
	//获取寄存器在栈中的存放地址
	intstkregs_t* regs = (intstkregs_t*)(thread->krlstktop-sizeof(intstkregs_t));
	//将寄存器结构清零
	memset((addr_t)regs,0,sizeof(intstkregs_t));
	//rip寄存器的值设置为程序运行入口地址
	regs->rip_old = (u64_t)entry;
	//cs寄存器的值设置为内核代码段选择子
	regs->cs_old = KRNL_CS_IDX;
	//设置内核态CPU寄存器初始值
	regs->rflgs = cpuflags;
	/**
	 * rsp寄存器的值为栈顶地址为什么指向栈顶？
	 * ss段选择子为什么为0？
	 * 为什么使用的不是krlstktop？
	 * 初步猜测：ss段寄存器是应用进程栈，而不是内核栈
	 */
	regs->rsp_old = thread->krlstktop;
	regs->ss_old = 0;

	/**
	 * 通用寄存器只需要清零即可，因为刚新建的进程还未向通用寄存器中写入数据
	 */

	//其他段寄存器设置为内核数据段选择子
	regs->ds = KRNL_DS_IDX;
	regs->es = KRNL_DS_IDX;
	regs->fs = KRNL_DS_IDX;
	regs->gs = KRNL_DS_IDX;

	/**
	 * 这里为什么要设置栈指针指向regs开始处？
	 */
	//设置进程下一次运行的指令地址为entry
	thread->context.nextrip = (u64_t)entry;
	//设置进程下一次的栈地址为regs
	thread->context.nextrsp = (u64_t)regs;

	return;
}

/**
 * 初始化程序用户栈
 */
static void user_stack_init(thread_t* thread,void* entry,cpuflag_t cpuflags){
	//处理栈顶16字节对齐
	thread->krlstktop &= (~0xf);
	thread->usrstktop &= (~0xf);

	//获取将intstkregs_t放入堆栈后的起始地址
	intstkregs_t *regs = (intstkregs_t*)(thread->krlstktop-sizeof(intstkregs_t));
	//将regs实例清零
	memset((addr_t)regs,0,sizeof(intstkregs_t));
	//rip寄存器的值设置为用户程序代码入口地址
	regs->rip_old = (u64_t)entry;
	//cs寄存器的值设置为用户程序代码段选择子
	regs->cs_old = USER_CS_IDX;
	//cpu标志寄存器设置为用户态初始值
	regs->rflgs = cpuflags;
	/**
	 * rsp寄存器设置为进程应用程序空间的栈
	 * 为什么使用的不是krlstktop？ 
	 */
	regs->rsp_old = thread->usrstktop;
	/**
	 * 这里为什么使用用户数据段选择子
	 */
	regs->ss_old = USER_DS_IDX;
	//初始化其他段选择子
	regs->ds = USER_DS_IDX;
	regs->es = USER_DS_IDX;
	regs->fs = USER_DS_IDX;
	regs->gs = USER_DS_IDX;

	//设置进程下一次运行的地址
	thread->context.nextrip = (u64_t)entry;
	//设置进程下一次运行的栈地址
	thread->context.nextrsp = (u64_t)regs;	
	return;
}

/**
 * 内核进程就是用进程的方式运行一段内核代码，这段代码可以参与并发
 */
static thread_t* krnl_thread_core(void* filerun,u64_t flag,u64_t privilege,u64_t priority,size_t usrstksz,size_t krlstksz){
	//分配内核栈空间
	addr_t krl_stack = mempool_new(krlstksz);
	if(NULL==krl_stack){
		//内核栈空间分配失败，直接返回
		return NULL;
	}

	//分配thread_t实例
	thread_t* thread = new_thread();
	if(NULL==thread){
		//创建失败，将先前申请的内核栈释放
		if(mempool_delete(krl_stack,krlstksz)==FALSE){
			//内核栈释放失败
			return NULL;
		}
	}

	//设置进程特权级
	thread->privilege = privilege;
	//设置进程优先级
	thread->priority = priority;
	
	//设置进程内核栈顶和内核栈开始地址
	thread->krlstkbase = krl_stack;			//指向分配的原始内存地址
	thread->krlstktop = krl_stack+krlstksz-1;	//指向分配栈内存最后一个字节
	
	//初始化内核进程的栈空间
	krnl_stack_init(thread,filerun,KRNLMOD_EFLAGS);
	
	//加入进程调度系统
	schedule_add(thread);
	
	return thread;
}

/**
 * 
 */
static thread_t* user_thread_core(void* filerun,u64_t flag,u64_t privilege,u64_t priority,size_t usrstksz,size_t krlstksz){
	//分配应用程序栈空间
	addr_t usr_stack = mempool_new(usrstksz);
	if(NULL == usr_stack){
		//用户栈分配失败
		return NULL;
	}

	//分配内核栈空间
	addr_t krl_stack = mempool_new(krlstksz);
	if(NULL == krl_stack){
		//将用户栈释放
		if(mempool_delete(usr_stack,usrstksz)==FALSE){
			//释放失败
			return NULL;
		}
		//内核栈分配失败，返回
		return NULL;
	}

	//创建thread_t实例变量
	thread_t *thread = new_thread();
	if(NULL==thread){ //进程创建失败
		//释放之前的栈空间
		if(mempool_delete(usr_stack,usrstksz)==FALSE || mempool_delete(krl_stack,krlstksz==FALSE)){
			return NULL;
		}
		return NULL;
	}

	//设置进程权限
	thread->privilege = privilege;
	//设置进程优先级
	thread->priority = priority;
	//设置进程内核栈顶和开始地址
	thread->krlstkbase = krl_stack;			//首字节
	thread->krlstktop = krl_stack+krlstksz-1;	//尾字节
	//设置进程的应用程序栈顶和开始地址
	thread->usrstkbase = usr_stack;
	thread->usrstktop = usr_stack+usrstksz-1;

	//初始化应用程序的内核栈和应用栈
	user_stack_init(thread,filerun,USERMOD_EFLAGS);

	//加入调度器系统
	schedule_add(thread);
		
	return thread;
}


/**
 * 根据指定的参数，创建一个进程
 * filerun: 可执行文件入口地址
 * flag: 创建标志
 * privilege: 特权级
 * priority: 优先级
 * usrstksz: 用户栈大小
 * krlstksz: 内核栈大小
 */
thread_t* krlnew_thread(void* filerun,u64_t flag,u64_t privilege,u64_t priority,size_t usrstksz,size_t krlstksz){
	size_t usz = usrstksz,ksz = krlstksz;
	//参数合法性检查(可执行文件地址|用户栈大小|内核栈大小)
	if(NULL==filerun || usz>THREAD_STACKSIZE_USER || ksz>THREAD_STACKSIZE_KRNL){
		return NULL;
	}
	//特权级检查(内核级别|用户级别),优先级检查
	if((THREAD_PRIVILEGE_USR!=privilege && THREAD_PRIVILEGE_SYS!= privilege) || priority>THREAD_PRIORITY_MAX){
		return NULL;
	}

	//如果内核栈/用户栈小于默认值，使用默认值
	if(ksz<THREAD_STACKSIZE_KRNL){
		ksz = THREAD_STACKSIZE_KRNL;
	}
	if(usz<THREAD_STACKSIZE_USER){
		usz = THREAD_STACKSIZE_USER;
	}

	//根据flag确定建立何种类型的进程
	if(THREAD_TYPE_KRNL == flag){		//创建内核进程
		return krnl_thread_core(filerun,flag,privilege,priority,usrstksz,krlstksz);
	} else if (THREAD_TYPE_USER == flag){	//创建用户进程
		return user_thread_core(filerun,flag,privilege,priority,usrstksz,krlstksz);
	} else return NULL;
}

//==================================================================================================================


