/**
 * CPU空转进程
 */

#include "globaltype.h"
#include "globalctrl.h"

//模拟某种资源的信号量
static sem_t sem;

/**
 * idle程序入口地址
 * 相当于一个内核的子程序，可以作为一个进程使用
 * 在空转任务中，做一些可维护性的任务，而不是真的无所事事
 * 空转进程会不停的轮询，执行进程调度函数，以便于有新进程到来是，可以立即响应，并切换到新进程
 */
static void idle_main(){
	while(TRUE){
		printk("@idle:");
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk("\n");
		krl_schedule();
	}
	
	return;
}


/**
 * 创建一个空转进程
 * 
 */
static thread_t* new_idle_thread(void* entry){
	//分配内核栈，使用默认内核栈大小
	addr_t krl_stack = mempool_new(THREAD_STACKSIZE_KRNL);
	if(NULL==krl_stack){
		//内核栈创建失败
		return NULL;	
	}
	//创建进程结构体变量
	thread_t* thread = new_thread();
	if(NULL==thread){
		if(mempool_delete(krl_stack,THREAD_STACKSIZE_KRNL)==FALSE){
			return NULL;
		}
		return NULL;
	}

	//设置进程特权级
	thread->privilege = THREAD_PRIVILEGE_SYS;
	//设置进程优先级
	thread->priority = THREAD_PRIORITY_MIN;
	//设置进程内核栈顶和开始地址
	thread->krlstkbase = krl_stack;
	thread->krlstktop = krl_stack + THREAD_STACKSIZE_KRNL-1;

	//初始化内核栈
	krnl_stack_init(thread,entry,KRNLMOD_EFLAGS);
	
	return thread;
}

/**
 * 新建一个空转进程
 */
static void idle_new(){
	//新建一个空转进程
	thread_t* idle = new_idle_thread(idle_main);
	if(NULL==idle){
		//创建失败，主动死机
		printk("new_idle error:thread=NULL\n");die(0);
	}

	//将新建的空闲进程添加到对应CPU的调度器中
	uint_t cpuid = hal_retn_cpuid();
	schedata_t *data = &scheclass.schedatas[cpuid];
	data->curthd = idle;	//设置idle为当前运行进程
	data->cpuidle = idle;	//设置idle为该CPU默认idle进程
	return ;
}

static void idle_start(){
	//
	uint_t cpuid = hal_retn_cpuid(); 
	schedata_t *data = &scheclass.schedatas[cpuid];

	//取得空转进程
	thread_t *thread = data->cpuidle;
	//设置空转进程的tss和R0特权级的栈
	thread->context.nexttss = &x64tss[cpuid];
	thread->context.nexttss->rsp0 = thread->krlstktop;
	//设置空转进程状态为运行状态
	thread->status = THREAD_STATUS_RUN;
	//启动进程运行
	first_sched(thread);
	
	return ;
}

void thread_a_main(){
	while(TRUE){
		printk("@A:");
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk("\n");
		
		sem_down(&sem);	//申请资源
		printk("using resource\n");die(0x200);	 //临界区
		sem_up(&sem);		//释放资源
	}
	return ;
}

void thread_b_main(){
	while(TRUE){
		printk("@B:");
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk("\n");
		
		//申请资源,申请失败将会阻塞在sem_down函数，等待被唤醒后重复执行P原语
		sem_down(&sem);	//申请资源
		printk("using resource\n");die(0x200);	 //临界区
		sem_up(&sem);		//释放资源
	}
	return ;
}

void thread_c_main(){
	for(uint_t i=0;;i++){
		printk("@C:");
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk(".");die(0x200);
		printk("\n");
		
		sem_down(&sem);	//申请资源
		printk("using resource\n");die(0x200);	 //临界区
		sem_up(&sem);		//释放资源
	}
	return ;
}

void init_abc_thread(){
	sem_t_init(&sem);
	sem_set(&sem,0,1);
	krlnew_thread(thread_a_main,0,THREAD_PRIVILEGE_SYS,0,THREAD_STACKSIZE_KRNL,THREAD_STACKSIZE_USER);
	krlnew_thread(thread_b_main,0,THREAD_PRIVILEGE_SYS,1,THREAD_STACKSIZE_KRNL,THREAD_STACKSIZE_USER);
	krlnew_thread(thread_c_main,0,THREAD_PRIVILEGE_SYS,2,THREAD_STACKSIZE_KRNL,THREAD_STACKSIZE_USER);
	return;
}

/**
 * 初始化空转进程
 */
void idle_init(){
	//为当前CPU新建一个空转进程
	idle_new();
	//启动多进程
	init_abc_thread();
	//启动空转进程
	idle_start();
	return ;	
}
