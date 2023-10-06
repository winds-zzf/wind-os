#ifndef __BUILDFILE_H
#define __BUILDFILE_H


#include "config.h"

/**
 * X86平台下的构造头文件
 */
#ifdef CFG_X86_PLATFORM

//硬件抽象层目标文件
#define BUILD_HALY_OBJS _hal.o hal.o vgastr.o stdio.o string.o utils.o global.o platform.o \
				_isr.o gdt.o idt.o tss.o i8259.o interrupt.o spinlock.o \
				remmu.o mmu.o memview.o mempage.o memtable.o memarea.o memobject.o memmgr.o memapi.o 
//内核目标文件
#define BUILD_KRNL_OBJS 	kernel.o knlglobal.o \
					kvmarea.o kvmbox.o kvmprocess.o mempool.o \
					thread.o schedule.o waitlist.o idle.o sem.o \
					device.o
//内存管理目标文件
#define BUILD_MEMY_OBJS 		//all relative *.o
//文件系统目标文件
#define BUILD_FSYS_OBJS 		//all relative *.o
//驱动程序目标文件
#define BUILD_DRIV_OBJS 		//all relative *.o
//函数库目标文件
#define BUILD_LIBS_OBJS 		//all relative *.o
//进程管理目标文件
#define BUILD_TASK_OBJS 		//all relative *.o

//链接对象(使用宏将所有用到的*.o都聚集在这里)
#define BUILD_LINK_OBJS BUILD_HALY_OBJS\
                     	BUILD_KRNL_OBJS\
                     	BUILD_MEMY_OBJS\
                        BUILD_FSYS_OBJS\
                        BUILD_DRIV_OBJS\
                        BUILD_LIBS_OBJS\
                        BUILD_TASK_OBJS
//链接器输入文件*.o
#define LINKR_IPUT_FILE BUILD_LINK_OBJS
//链接器输出文件kernel.elf
#define LINKR_OPUT_FILE kernel.elf
//链接输出文件kernel.elf
#define KERNL_ELFF_FILE LINKR_OPUT_FILE
//最终模块文件kernel.bin
#define KERNL_BINF_FILE kernel.bin

#endif //CFG_X86_PLATFORM



/**
 * x64平台下的构造头文件
 */
#ifdef CFG_X64_PLATFORM

#endif //FG_X64_PLATFORM

/**
 * 其他平台的构造头文件
 */


#endif // __BUILDFILE_H
