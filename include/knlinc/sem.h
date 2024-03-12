/**
 * 信号量模块
 * API定义
 */ 
#ifndef __SEM_H
#define __SEM_H

/**
 * 初始化sem结构体
 */
void sem_t_init(sem_t* sem);

/**
 * 设置信号量的值
 * 信号量的值描述了同种资源的数量，如果计数为0，此时不再减少，也可以改为负值
 */
void sem_set(sem_t* sem,uint_t flag,uint_t count);


/**
 * 信号量P原语
 * 
 * 尝试申请sem对应的资源，
 * 如果申请失败，将进程加入到该资源的等待队列，并进行进程调度
 */
void sem_down(sem_t* sem);

/**
 * 信号量V原语，释放占用的资源，同时将所有阻塞进程添加到就绪对列，重新等待调度
 */
void sem_up(sem_t* sem);


#endif //__SEM_H