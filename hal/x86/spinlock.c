/**
 * CPU控制模块
 * API实现
 */
#include"globaltype.h"
#include"globalctrl.h"

void spinlock_init(Spinlock *lock){
	lock->lock = 0;
	return;
}

/**
 * 使用了xchg指令来交换lock变量和1的值。
 * 如果lock原本的值为0，说明锁是空闲的，可以获取锁。
 * 如果lock原本的值不为0，说明锁已经被其他线程持有，需要一直循环等待，直到获取到锁。
 * 
 * lock:自旋锁
 * 事实上cmpl $0,%1不会被常常执行,这是为了不在cpu指令高速缓存中填充无用代码,
 * 要知道那可是用每位6颗晶体管做的双极性静态,储存器,比内存条快几千个数量级
 */
void spinlock_lock(Spinlock *lock){
	__asm__ __volatile__(
		"1:         \n"		//标签1
		"lock; xchg  %0, %1 \n"	//使用lock前缀进行原子交换指令：lock先存入%0后赋值1
		"cmpl   $0, %0      \n"	//lock==0?
		"jnz    2f      	\n"	//lock==1，跳转到2
		".section .spinlock.text,"
		"\"ax\""
		//重新定义一个代码段所以jnz 2f下面并不是cmpl   $0, %1
		"\n"
		//自旋等待：while(lock==1);
		"2:         		\n"  //标签2
		"cmpl   $0, %1     	\n" 	//lock==1?
		"jne    2b      	\n"	//lock==1,跳转到2
		"jmp    1b      	\n"	//lock!=1,跳转到1
		".previous      	\n"	//恢复之前的代码段
		:
		: "r"(1), "m"(*lock)	//输入操作数，将1作为%0的初始值，将*lock作为%1的初始值
	);
	return;
}

/**
 * 对自旋锁解锁
 * lock：自旋锁
 */
void spinlock_unlock(Spinlock *lock){
	__asm__ __volatile__(
		"movl   $0, %0\n"	//将0存储在lock变量所在的内存地址%0中，表示释放锁
		:
		: "m"(*lock)		//输出操作数，将%0（即*lock）输出
	);
	return;
}

void spinlock_lock_saveflg_cli(Spinlock *lock, cpuflag_t *cpuflag){
	__asm__ __volatile__(
		"pushfq             \n\t"
		"cli                \n\t"
		"popq %0            \n\t"
		
		"1:                 \n\t"
		"lock; xchg  %1, %2 \n\t"
		"cmpl   $0,%1       \n\t"
		"jnz    2f          \n\t"
		".section .spinlock.text,"
		"\"ax\""
		"\n\t"                    //重新定义一个代码段所以jnz 2f下面并不是
		"2:                 \n\t" //cmpl $0,%1 事实上下面的代码不会常常执行,
		"cmpl   $0,%2       \n\t" //这是为了不在cpu指令高速缓存中填充无用代码
		"jne    2b          \n\t"
		"jmp    1b          \n\t"
		".previous          \n\t"
		: "=m"(*cpuflag)
		: "r"(1), "m"(*lock)
	);
	return;
}

void spinlock_unlock_restflg_sti(Spinlock *lock, cpuflag_t *cpuflag){
	__asm__ __volatile__(
		"movl   $0, %0\n\t"
		"pushq %1 \n\t"
		"popfq \n\t"
		:
		: "m"(*lock), "m"(*cpuflag)
	);
	return;
}

void spinlock_lock_cli(Spinlock *lock, cpuflag_t *cpuflag){
	__asm__ __volatile__(
		"pushfq             \n\t"
		"cli                \n\t"
		"popq %0            \n\t"
		
		"1:                 \n\t"
		"lock; xchg  %1, %2 \n\t"
		"cmpl   $0,%1       \n\t"
		"jnz    2f          \n\t"
		".section .spinlock.text,"
		"\"ax\""
		"\n\t"                    //重新定义一个代码段所以jnz 2f下面并不是
		"2:                 \n\t" //cmpl $0,%1 事实上下面的代码不会常常执行,
		"cmpl   $0,%2       \n\t" //这是为了不在cpu指令高速缓存中填充无用代码
		"jne    2b          \n\t"
		"jmp    1b          \n\t"
		".previous          \n\t"
		: "=m"(*cpuflag)
		: "r"(1), "m"(*lock)
	);
	return;
}

void spinlock_unlock_sti(Spinlock *lock, cpuflag_t *cpuflag){
	__asm__ __volatile__(
		"movl   $0, %0\n\t"
		"pushq %1 \n\t"
		"popfq \n\t"
		:
		: "m"(*lock), "m"(*cpuflag)
	);
	return;
}

