/**
 * 中断控制模块
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

/**
 * · 栈帧：元素大小指的是内核栈中每个栈帧（stack frame）的大小。
 * 	栈帧是用于存储函数调用的局部变量、返回地址和其他与函数调用相关的信息的一块内存区域。
 * ·
 */
static void set_intFault(IntFault *ifault,u32_t flag,u32_t status,uint_t priority,uint_t irq);
static void init_intFaults();


void interrupt_init(){
	//1.初始化gdt描述符
	gdt_init();
	printk("finish initializing gdt...\n");
	
	//2.初始化idt描述符
	idt_init();
	printk("finish initializing idt...\n");
	
	//3.初始化中断异常描述符
	init_intFaults();
	printk("finish initializing intfault...\n");

	//4.初始化8259A中断控制器
	i8259_init();
	printk("finish initializing 8259A...\n");
	
	return;
}

void interrupt_display(){


	
	return;
}


/**
 * 调用中断处理的回调函数
 * number: 中断异常描述符编号
 * sframe: 栈帧指针
 * 
 * CPU在进行中断时会自动将一部分寄存器保存在堆栈中，如PC/PSW/FLAGS等，还可能会保存SP/BP寄存器
 * 中断回调函数需要借助这些寄存器值来处理中断，因此中断回调函数需要sframe这个参数
 */
static void run_callback(uint_t ifault_num, void *sframe){	
	//1.根据中断号获取中断异常描述符
	IntFault *ifault = get_intFault(ifault_num);

	//2.遍历中断异常描述符对应的中断服务例程链表
	intservice_t *iservice = NULL;
	list_t *pos = NULL;	//链表指针
	list_for_each(pos, &ifault->servlst){	//责任链模式:遍历链表，直到找到匹配的回调函数
		//获取中断服务例程
		iservice = list_entry(pos, intservice_t, inthook);
		//调用中断服务例程中的回调函数(是否进行处理，在回调函数执行时自行判断)
		iservice->entry(ifault_num, iservice->dev, sframe);
	}
	return;
}


/**
 * 中断处理函数
 * int_num: 异常号
 * sframe: 栈帧指针
 * 
 * 这里把中断和异常统称为中断，因为它们的处理方式相同
 * 为什么调用中断回调函数之前要加锁？
 */
static void hal_do_hwint(uint_t intnum,void *sframe){
	IntFault *ifault = NULL;
	cpuflag_t cpuflag;		//CPU标志寄存器

	/**
	 * 为什么要同时上锁、保存寄存器并关中断？？？？
	 */
	//1.根据中断号获取中断异常描述符
	ifault = get_intFault(intnum);
	//2.中断异常描述符加锁并关中断，
	spinlock_lock_saveflg_cli(&ifault->lock,&cpuflag);
	//3.修改中断异常描述信息
	ifault->count++;
	ifault->deep++;
	//4.运行中断处理的回调函数
	run_callback(intnum,sframe);		//中断回调函数具有原子性
	//5.修改中断描述符信息
	ifault->deep--;
	//6.解锁并恢复中断状态
	spinlock_unlock_restflg_sti(&ifault->lock,&cpuflag);
	
	return;
}

/**
 * 系统调用分发器
 * sys_num: 系统调用号
 * sframe: 栈帧指针：也就是栈顶指针rsp的值
 * return:
 */
sysstus_t hal_syscal_allocator(uint_t sys_num,void* sframe){
	cpuflag_t cpuflag;
	save_flags_sti(&cpuflag);

	//调用系统服务分发器函数
	sysstus_t rets = krlservice(sys_num,sframe); 

	restore_flags_cli(&cpuflag);
	return rets;
}

/**
 * 异常分发器
 * fault_num: 异常号
 * sframe: 栈帧指针
 * 
 */
void hal_fault_allocator(uint_t fault_num,void *sframe){
	printk("faultnumb is :%d\n",fault_num);
	
	cpuflag_t cpuflag;
	save_flags_sti(&cpuflag);

	/**
	 * 暂时不处理多种类型的异常
	 */
	//hal_do_hwint(fault_num,sframe);
	
	/**
	 * 暂时演示处理缺页异常
	 */
	addr_t vadr = NULL;
	if(fault_num==14){
		//获取缺页地址
		vadr = (addr_t)read_cr2();
		printk("fault address:0x%lx\n",vadr);
		if(krluserspace_accessfailed(vadr)!=0){
			error("page fault process error");
		}
		restore_flags_cli(&cpuflag);
		return;	//处理成功
	}

	die(0);
	/**
	 * 如果缺页异常处理失败，那么撤销当前进程的执行
	 */

	restore_flags_cli(&cpuflag);
	return;
}

/**
 * CPU收到中断信号之后，找到中断门IntGate结构体，根据中断门中的地址找到异常处理程序入口
 * 异常处理程序在保存了CPU信息之后，调用hal_do_allocator,并将传入中断号和栈帧指针
 */
void hal_hwint_allocator(uint_t intnum, void *sframe){
	cpuflag_t cpuflag;
	save_flags_sti(&cpuflag);
	
	/**
	 * 为什么要在中断处理之前就通知8259a中断处理完毕了？？？？
	 * 不应该在中断处理结束之后吗？
	 */
	i8259_send_eoi();
	hal_do_hwint(intnum,sframe);
	
	/**
	 * 为什么要在这个位置检查是否需要调度？？？
	 */
	schedule_check();

	restore_flags_cli(&cpuflag);
	return;
}


/**
 * 设置一个中断异常描述符的信息
 * ifault: 中断异常描述符
 * flag: 
 * status: 
 * priority: 中断特权级
 * irq: 中断请求信号
 */
static void set_intFault(IntFault *ifault,u32_t flag,u32_t status,uint_t priority,uint_t irq){
	//
	spinlock_init(&ifault->lock);
	ifault->flag = flag;
	ifault->status = status;
	//
	ifault->priority = priority;
	ifault->irq = irq;
	ifault->deep = 0;
	ifault->count = 0;
	//
	list_t_init(&ifault->servlst);
	ifault->servnum = 0;
	//
	list_t_init(&ifault->thdlst);
	ifault->thdnum = 0;
	ifault->thd = NULL;
	ifault->rbtree = NULL;
	
	return;
}

/**
 * 初始化中断异常描述符表
 * 
 * 为什么priority字段设置成了index
 */
static void init_intFaults(){
	for(uint_t i=0;i<INTMAX;i++){	//依次初始化所有中断异常描述符
		set_intFault(&intFaults[i],0,0,i,i);
	}
	return;
}

/**
 * 获取中断异常描述符IntFault
 * irq_num:中断请求号interrupt request
 * return:
 * 
 */
IntFault* get_intFault(uint_t irq_num){
    if (irq_num > INTMAX){		//中断请求号检查[0,INTMAX]
        return NULL;
    }
    return &intFaults[irq_num];
}

/**
 * 默认初始化：所有字段都使用默认值
 * 赋值初始化：将指定字段初始化为指定值
 */
void intservice_t_init(intservice_t *iserv,u32_t flag,IntFault *ifault,void* device,IntCallback entry){
	iserv->flag = flag;
	iserv->index = 0;
	list_t_init(&iserv->inthook);
	iserv->ifault = ifault;
	list_t_init(&iserv->devhook);
	iserv->dev = device;
	iserv->entry = entry;
	return;
}


bool_t add_intservice(IntFault *ifault,intservice_t* iservice){
	if(NULL==ifault || NULL==iservice){
		return FALSE;
	}
	cpuflag_t cpuflag;
	spinlock_lock_saveflg_cli(&ifault->lock,&cpuflag);
	
	list_add(&iservice->inthook,&ifault->servlst);
	ifault->servnum++;	//中断回调链表长度加1
	/**
	 * 为什么没有把设备也挂载上去？
	 */
	spinlock_unlock_restflg_sti(&ifault->lock,&cpuflag);	
	return TRUE;
}

drvstus_t hal_enable_intline(u32_t intnum){
	//检查intnum是否是硬件中断
	if(intnum > 16){
		return DFCERRSTUS;
	}
	
	//调用中断控制器函数开启指定信号线上的中断
	i8259_enable_line(intnum);
	return DFCOKSTUS;
}

drvstus_t hal_disable_intline(u32_t intnum){
	//检查intnum是否是硬件中断
	if(intnum > 16){
		return DFCERRSTUS;
	}
	
	//调用中断控制器函数屏蔽指定信号线上的中断
	i8259_disable_line(intnum);
	return DFCOKSTUS;
}
