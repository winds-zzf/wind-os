/**
 * CPU空转进程
 */

#include "globaltype.h"
#include "globalctrl.h"

//模拟某种资源的信号量
static sem_t sem1;
static sem_t sem2;
static sem_t sem3;

/**
 * idle程序入口地址
 * 相当于一个内核的子程序，可以作为一个进程使用
 * 在空转任务中，做一些可维护性的任务，而不是真的无所事事
 * 空转进程会不停的轮询，执行进程调度函数，以便于有新进程到来是，可以立即响应，并切换到新进程
 */
static void idle_main(){
	uint_t cpuid = hal_retn_cpuid();
	schedata_t* data = &scheclass.schedatas[cpuid];
	while(TRUE){
		printk("@idle:%d,%d,%d\n",sem1.count,sem2.count,sem3.count);
		printk("num:%d\n",data->threadNum);
		printk("sem1.num:%d\n",sem1.wlst.waitsNum);
		printk("sem2.num:%d\n",sem2.wlst.waitsNum);
		printk("sem3.num:%d\n",sem3.wlst.waitsNum);
	}
	return;
}

void thread_a_main(){
	while(TRUE){
		CLI();
		sem_down(&sem1);
		STI();
		printk("A");
		CLI();
		sem_up(&sem2);
		STI();
	}
	
	return ;
}

void thread_b_main(){
	while(TRUE){
		CLI();
		sem_down(&sem2);
		STI();
		printk("B");
		CLI();
		sem_up(&sem3);
		STI();
	}
	
	return ;
}

void thread_c_main(){
	while(TRUE){
		CLI();
		sem_down(&sem3);
		STI();
		printk("C");
		CLI();
		sem_up(&sem1);
		STI();
	}
	return ;
}


/**
 * 创建一个空转进程，空转进程属于内核进程，处在内核文件只中
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

void init_abc_thread(){
	sem_t_init(&sem1);
	sem_t_init(&sem2);
	sem_t_init(&sem3);	
	sem_set(&sem1,0,0);
	sem_set(&sem2,0,0);
	sem_set(&sem3,0,0);
	
	krlnew_thread(thread_a_main,0,THREAD_PRIVILEGE_SYS,2,THREAD_STACKSIZE_KRNL,THREAD_STACKSIZE_USER);
	krlnew_thread(thread_b_main,0,THREAD_PRIVILEGE_SYS,2,THREAD_STACKSIZE_KRNL,THREAD_STACKSIZE_USER);
	krlnew_thread(thread_c_main,0,THREAD_PRIVILEGE_SYS,2,THREAD_STACKSIZE_KRNL,THREAD_STACKSIZE_USER);
	sem_up(&sem1);
	return;
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

/**
 * 对于用户态进程来说，出于程序设计方便和内存安全的角度等原因，
 * 为每个用户态进程引入了独立的虚拟地址空间，其被映射到用户空间。
 * 用户进程，平时运行在用户态，有自己的虚拟地址空间，但是可以通过中断、系统调用等内陷到内核态。
 * 内核进程，没有独立的地址空间，所有内核线程的地址空间都是一样的，没有自己的地址空间，
 * 所以它们的”current->mm”都为空，其运行在内核空间，本身就是内核的一部分或者说是内核的分身。
 * 
 * 有些系统中专门为全局中断处理提供了中断栈，但是x86中并没有中断栈，中断在当前进程的内核栈中处理。
 */
