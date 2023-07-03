#ifndef BUILDFILE_H
#define BUILDFILE_H
#include "config.h"

#ifdef CFG_X86_PLATFORM
#define BUILD_HALY_OBJS _hal.o hal.o vgastr.o stdio.o string.o utils.o platform.o memory.o interrupt.o global.o allocate.o
#define BUILD_KRNL_OBJS
#define BUILD_MEMY_OBJS
#define BUILD_FSYS_OBJS
#define BUILD_DRIV_OBJS
#define BUILD_LIBS_OBJS
#define BUILD_TASK_OBJS

//链接对象(将所有文件链接到一起，构成内核文件)
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

                          
#endif // BUILDFILE_H
