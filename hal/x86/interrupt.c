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


static void set_intFault(IntFault *ifault,u32_t flag,u32_t sts,uint_t priority,uint_t irq);
static void init_intFaults();

static void hal_do_hwint(uint_t int_num,void *sframe);
static IntFault* get_intFault(uint_t irq_num);
static void run_callback(uint_t ifault_num,void *sframe);


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

	//5.取消mask，开启硬件中断请求
	//i8259_enabled_line(0);
	
	return;
}

void interrupt_display(){


	
	return;
}

/**
 * 系统调用分发器
 * sys_num: 系统调用号
 * msgp: 消息指针
 * return:
 */
sysstus_t hal_syscal_allocator(uint_t sys_num,void* msgp){
	;
	return 0; 
}

/**
 * 异常分发器
 * fault_num: 异常号
 * sframe: 栈帧指针
 * 
 */
void hal_fault_allocator(uint_t fault_num,void *sframe){
	/**
	 * 暂时演示处理缺页中断
	 */
	addr_t vadr = NULL;
	printk("faultnumb is :%d\n",fault_num);
	if(fault_num==14){
		//获取缺页地址
		vadr = (addr_t)read_cr2();
		printk("fault address:0x%lx\n",vadr);
		if(krluserspace_accessfailed(vadr)!=0){
			printk("page fault process error\n");
			die(0);
		}
		//成功则返回
		return ;
	}

	die(0);	//如果缺页异常没有被解决，将会重复出现缺页异常
	//我们的异常处理回调函数也是放在中断异常描述符中的
	hal_do_hwint(fault_num,sframe);
	
	return;
}

/**
 * CPU收到中断信号之后，找到中断门IntGate结构体，根据中断门中的地址找到异常处理程序入口
 * 异常处理程序在保存了CPU信息之后，调用hal_do_allocator,并将传入中断号和
 */
void hal_hwint_allocator(uint_t int_num, void *sframe){
	i8259_send_eoi();
	hal_do_hwint(int_num,sframe);
	die(0);
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
static void hal_do_hwint(uint_t int_num,void *sframe){
	printk("interrupt occupied:%d\n",int_num);
	
	IntFault *ifault = NULL;
	cpuflag_t cpuflag;		//CPU标志寄存器
	//0.参数检查
	;
	//1.根据中断号获取中断异常描述符
	ifault = get_intFault(int_num);
	//2.中断异常描述符加锁并关中断，
	spinlock_lock_saveflg_cli(&ifault->lock,&cpuflag);
	//3.修改中断异常描述信息
	ifault->count++;
	ifault->deep++;
	//4.运行中断处理的回调函数
	run_callback(int_num,sframe);		//中断回调函数具有原子性
	//5.修改中断描述符信息
	ifault->deep--;
	//6.解锁并恢复中断状态
	spinlock_unlock_restflg_sti(&ifault->lock,&cpuflag);
	
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
	IntHandle *ihandle;
	List *lst;	//链表指针
	
	//1.根据中断号获取中断异常描述符
	IntFault *ifault = get_intFault(ifault_num);
	//2.遍历中断异常描述符的回调函数链表
	list_for_each(lst, &ifault->callback_list){	//责任链模式:遍历链表，直到找到匹配的回调函数
		//获取回调函数链表上对象即intserdsc_t结构
		ihandle = list_entry(lst, IntHandle, int_list);	//已知结构体指定成员地址计算结构体地址
		//调用中断处理回调函数(是否进行处理，在回调函数执行时自行判断)
		ihandle->entry(ifault_num, ihandle->device, sframe);
	}
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
	list_init(&ifault->callback_list);
	ifault->callback_num = 0;
	//
	list_init(&ifault->thread_list);
	ifault->thread_num = 0;
	ifault->thread = NULL;
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
static IntFault *get_intFault(uint_t irq_num){
    if (irq_num > INTMAX){		//中断请求号检查[0,INTMAX]
        return NULL;
    }
    return &intFaults[irq_num];
}


void init_intHandle(IntHandle *ihandle,u32_t flag,IntFault *ifault,void* device,IntCallback entry){
	ihandle->flag = flag;
	ihandle->index = 0;
	list_init(&ihandle->int_list);
	ihandle->intFault = ifault;
	list_init(&ihandle->device_list);
	ihandle->device = device;
	ihandle->entry = entry;
	return;
}


bool_t add_ihandle(IntFault *ifault,IntHandle *ihandle){
	if(NULL==ifault||NULL==ihandle){
		return FALSE;
	}
	cpuflag_t cpuflag;
	spinlock_lock_saveflg_cli(&ifault->lock,&cpuflag);
	list_add(&ihandle->int_list,&ifault->callback_list);
	ifault->callback_num++;	//中断回调链表长度加1

	spinlock_unlock_restflg_sti(&ifault->lock,&cpuflag);	
	return TRUE;
}
