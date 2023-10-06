/**
 * 进程调度器
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"


/**
 * 
 */
static void thdlst_t_init(thdlst_t* threads){
	list_init(&threads->threads);	//初始化空进程链表
	threads->threadsNum = 0;
	threads->curthd= NULL;		//没有正在运行的进程
	
	return ;
}


/**
 * 
 */
static void schedata_t_init(schedata_t* data){
	spinlock_init(&data->lock);
	data->cpuid = 0;
	data->flags = NOTS_SCHED_FLGS;
	data->count = 0;
	data->threadNum = 0;
	data->priority = 0;		//最高优先级
	
	data->cpuidle = NULL;	//开始没有空转进程和运行的进程
	data->curthd = NULL;

	//初始化schedata中的每一个thdlst
	for(uint_t i=0;i<THREAD_PRIORITY_MAX;i++){
		thdlst_t_init(&data->thdlst[i]);
	}

	return ;
}

/**
 * 
 */
static void scheclass_t_init(scheclass_t* class){
	spinlock_init(&class->lock);
	class->cpuNum = 0;
	class->thdNum = 0;
	class->threadid_inc = 0;
	for(uint_t i=0;i<CPUCORE_MAX;i++){
		schedata_t_init(&class->schedatas[i]);
	}
	
	return ;
}

/**
 * 初始化进程调度器
 */
void schedule_init(){
	//初始化进程调度器
	scheclass_t_init(&scheclass);
	
	return ;
}

/**
 * 获取空转进程，每个CPU上都有一个空转进程
 */
static thread_t* idle_thread(){
	//根据cpuid找到指定cpu的调度器
	uint_t cpuid = hal_retn_cpuid();
	schedata_t *data = &scheclass.schedatas[cpuid];
	//获取该cpu的空转进程
	if(NULL==data->cpuidle){
		printk("error in idle_thread\n");
		die(0);	//挂机
	}
	
	return data->cpuidle; 
}

/**
 * 获取当前正在运行的进程
 */
static thread_t* current_thread(){
	/**
	 * 获取cpuid
	 * 为什么要获取当前的cpu id？
	 */
	uint_t cpuid = hal_retn_cpuid();
	//通过CPU的id获取当前CPU的调度结构
	schedata_t *data = &scheclass.schedatas[cpuid];
	if(NULL==data->curthd){
		//要么就是空转进程，要么就是其他进程
		printk("schedata->current==NULL");
		die(0);
	}
	
	return data->curthd;
}

/**
 * 进程调度算法：类似于多级反馈队列
 */
static thread_t* select_thread(){
	thread_t* thread = NULL;
	thdlst_t *queue = NULL;
	uint_t cpuid = hal_retn_cpuid();	//获取cpuid
	schedata_t *data = &scheclass.schedatas[cpuid];	//选择对应cpu的调度器

	//进程调度时cpu要关中断
	cpuflag_t cpuflag;
	spinlock_lock_cli(&data->lock,&cpuflag);

	//遍历调度器各级优先队列
	for(uint_t prity=0;prity<THREAD_PRIORITY_MAX;prity++){
		queue = &data->thdlst[prity];
		//找到首个非空优先队列
		if(queue->threadsNum>0){
			/**
			 * 实际检查队列是否为空，为什么还要重复检查？？？
			 */
			if(list_is_empty_careful(&queue->threads)==FALSE){
				//获取优先队列的首个进程
				thread = list_entry(queue->threads.next,thread_t,hook);
				list_del(&thread->hook);
				//将当前优先队列上正在运行的进程放到队列尾
				if(NULL!=queue->curthd){
					list_add_tail(&queue->curthd->hook,&queue->threads);
				}
				//将选择的进程更新为当前进程
				queue->curthd = thread;
				goto out;
			}
			//队列为空
			if(NULL!=queue->curthd){
				thread = queue->curthd;
				goto out;
			}
		}
		//继续寻找下一个
	}

	//如果最终也没有找到，那就使用空转进程
	/**
	 * 为什么将调度器的优先级设置为最大？
	 */
	data->priority = THREAD_PRIORITY_MIN;
	thread = idle_thread();	//获取空转进程
out:
	spinlock_unlock_sti(&data->lock,&cpuflag);
	return thread; 
}

/**
 * 首次运行进程
 */
void first_sched(thread_t* thread){
	__asm__ __volatile__(
		"movq %[NEXT_RSP],%%rsp\n\t"  //设置CPU的RSP寄存器为该进程机器上下文结构中的RSP
		//恢复进程保存在内核栈中的段寄存器
		"popq %%r14\n\t"
		"movw %%r14w,%%gs\n\t"
		"popq %%r14\n\t"
		"movw %%r14w,%%fs\n\t"
		"popq %%r14\n\t"
		"movw %%r14w,%%es\n\t"
		"popq %%r14\n\t"
		"movw %%r14w,%%ds\n\t"
		//恢复进程保存在内核栈中的通用寄存器
		"popq %%r15\n\t"
		"popq %%r14\n\t"
		"popq %%r13\n\t"
		"popq %%r12\n\t"
		"popq %%r11\n\t"
		"popq %%r10\n\t"
		"popq %%r9\n\t"
		"popq %%r8\n\t"
		"popq %%rdi\n\t"
		"popq %%rsi\n\t"
		"popq %%rbp\n\t"
		"popq %%rdx\n\t"
		"popq %%rcx\n\t"
		"popq %%rbx\n\t"
		"popq %%rax\n\t"
		//恢复进程保存在内核栈中的RIP、CS、RFLAGS，（有可能需要恢复进程应用程序的RSP、SS）寄存器
		"iretq\n\t"
		:
		: [ NEXT_RSP ] "m"(thread->context.nextrsp)
		: "memory"
	);
	return;
}

/**
 * 进入下一个机器上下文
 */
void __to_new_context(thread_t* next,thread_t* prev){
	//获取cpuid并确定使用哪个CPU的调度器
	uint_t cpuid = hal_retn_cpuid();
	schedata_t *data = &scheclass.schedatas[cpuid];

	//将下一个运行的进程设置为当前进程
	data->curthd = next;
	//设置下一个运行进程的tss为当前CPU的tss
	next->context.nexttss = &x64tss[cpuid];
	//设置当前CPU的tss中的R0栈为下一个运行进程的内核栈
	next->context.nexttss->rsp0 = next->krlstktop;
	//装载下一个运行进程的MMU页表
	mmu_load(&next->mem->mmu);
	//如果下一个进程第一次运行，需要特殊处理
	if(THREAD_STATUS_NEW==next->status){
		//进程修改为运行态
		next->status = THREAD_STATUS_RUN;
		first_sched(next);
	}	
		
	return;
}
	

/**
 * 进程切换函数
 */
static void switch_thread(thread_t* prev,thread_t *next){
	__asm__ __volatile__(
		"pushfq \n\t"//保存当前进程的标志寄存器
		"cli \n\t"  //关中断
		//保存当前进程的通用寄存器
		"pushq %%rax\n\t"
		"pushq %%rbx\n\t"
		"pushq %%rcx\n\t"
		"pushq %%rdx\n\t"
		"pushq %%rbp\n\t"
		"pushq %%rsi\n\t"
		"pushq %%rdi\n\t"
		"pushq %%r8\n\t"
		"pushq %%r9\n\t"
		"pushq %%r10\n\t"
		"pushq %%r11\n\t"
		"pushq %%r12\n\t"
		"pushq %%r13\n\t"
		"pushq %%r14\n\t"
		"pushq %%r15\n\t"
		//保存CPU的RSP寄存器到当前进程的机器上下文结构中
		"movq %%rsp,%[PREV_RSP] \n\t"
		//把下一个进程的机器上下文结构中的RSP的值，写入CPU的RSP寄存器
		"movq %[NEXT_RSP],%%rsp \n\t"//事实上这里已经切换到下一个进程了，因为切换进程的内核栈    
		//调用__to_new_context函数切换MMU页表
		"callq __to_new_context\n\t "		//__to_new_context不能是static，否则这里将找不到
		//恢复下一个进程的通用寄存器
		"popq %%r15\n\t"
		"popq %%r14\n\t"
		"popq %%r13\n\t"
		"popq %%r12\n\t"
		"popq %%r11\n\t"
		"popq %%r10\n\t"
		"popq %%r9\n\t"
		"popq %%r8\n\t"
		"popq %%rdi\n\t"
		"popq %%rsi\n\t"
		"popq %%rbp\n\t"
		"popq %%rdx\n\t"
		"popq %%rcx\n\t"
		"popq %%rbx\n\t"
		"popq %%rax\n\t"
		"popfq \n\t"      //恢复下一个进程的标志寄存器
		//输出当前进程的内核栈地址
		: [ PREV_RSP ] "=m"(prev->context.nextrsp)
		//读取下一个进程的内核栈地址
		: [ NEXT_RSP ] "m"(next->context.nextrsp), "D"(next), "S"(prev)//为调用__to_new_context函数传递参数
		: "memory"
    );
    return;
}


/**
 * 
 */
void krl_schedule(){
	printk("schedule\n");
	/**
	 * 参考Cosmos里面是如何避免频繁调度同一个idle的
	 */



	 
	//返回当前运行的进程
	thread_t* prev = current_thread();
	//选择下一个运行的进程
	thread_t* next = select_thread();
	//执行进程切换
	switch_thread(prev,next);
	
	return;
}

/**
 * 将线程添加到进程调度
 */
void schedule_add(thread_t *thread){
	uint_t cpuid = hal_retn_cpuid();
	schedata_t* data = &scheclass.schedatas[cpuid];

	//处理cpu调度器
	cpuflag_t cpuflag=0;
	spinlock_lock_cli(&data->lock,&cpuflag);
	//根据待加入线程的优先级找到指定的优先队列
	thdlst_t* lst = &data->thdlst[thread->priority];
	list_add_tail(&thread->hook,&lst->threads);
	lst->threadsNum++; //优先队列总进程数+1
	data->threadNum++;	//调度器总进程数+1
	spinlock_unlock_sti(&data->lock,&cpuflag);

	//修改全局调度器进程数
	spinlock_lock_cli(&scheclass.lock,&cpuflag);
	scheclass.thdNum++;	//全局调度器进程计数+1
	spinlock_unlock_sti(&scheclass.lock,&cpuflag);
	
	return ;
}


/**
 * 将一个进程阻塞，挂入等待队列
 */
void schedule_wait(waitlist_t *wlst){
	//获取当前调度需要的CPU
	uint_t cpuid = hal_retn_cpuid();
	schedata_t *data = &scheclass.schedatas[cpuid];

	//获取当前工作的线程,及优先级
	thread_t* curthd = current_thread();
	u64_t prity = curthd->priority;

	//当前CPU调度器上锁，避免其他CPU使用该调度器,并避免当前CPU中断
	cpuflag_t sche_cpuflag;
	spinlock_lock_cli(&data->lock,&sche_cpuflag);

	//当前线程上锁
	cpuflag_t thd_cpuflag;
	spinlock_lock_cli(&curthd->lock,&thd_cpuflag);
	//当前运行进程设置为等待状态，并从就绪队列删除
	curthd->status = THREAD_STATUS_WAIT;
	/**
	 * 有个问题时，当前运行进程是不在就绪队列中的，这里为什么还要脱链
	 */
	list_del(&curthd->hook);
	//当前线程释放锁
	spinlock_unlock_sti(&curthd->lock,&thd_cpuflag);

	//如果正在运行的进程是优先队列的当前进程，则置空，优先队列长度-1
	if(data->thdlst[prity].curthd==curthd){
		data->thdlst[prity].curthd=NULL;
	}
	data->thdlst[prity].threadsNum--;
	
	//CPU调度器释放锁
	spinlock_unlock_sti(&data->lock,&sche_cpuflag);

	//将当前进程添加到指定的等待队列
	waitlist_add(wlst,curthd);
	
	return;
}

/**
 * 从等待队列中获得一个进程，上CPU运行
 */
void schedule_up(waitlist_t* wlst){
	uint_t cpuid = hal_retn_cpuid();
	schedata_t *data = &scheclass.schedatas[cpuid];

	//从指定等待队列wlst中取出一个进程，放到就绪队列尾部，等待调度
	thread_t* thd = waitlist_del(wlst);
	if(NULL==thd){
		printk("thd==NULL in schedule_up\n");
		goto err_step;
	}
	u64_t prity = thd->priority;
	if(prity>=THREAD_PRIORITY_MAX){
		printk("privilege too big in schedule_up\n");
		goto err_step;
	}

	//该CPU调度器上锁,并将CPU关中断
	cpuflag_t sche_cpuflag;
	spinlock_lock_cli(&data->lock,&sche_cpuflag);

	//线程上锁，并将CPU关中断
	cpuflag_t thd_cpuflag;
	spinlock_lock_cli(&thd->lock,&thd_cpuflag);
	thd->status = THREAD_STATUS_READY;		//改为就绪态，并放入就绪对列
	//将进程从等待队列取出，放到了优先队列尾部，等待调度
	list_add_tail(&thd->hook,&data->thdlst[prity].threads);
	spinlock_unlock_sti(&thd->lock,&thd_cpuflag);
	
	//优先队列长度+1
	data->thdlst[prity].threadsNum++;
	
	spinlock_unlock_sti(&data->lock,&sche_cpuflag);
	return;	//正常返回、
	
//错误处理
err_step:
	{printk("schedule_up error\n");die(0);}
	return;
}

//=================================================================================================================
























