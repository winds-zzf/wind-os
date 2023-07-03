/*###################################################################
#		hal自动化编译配置文件 Makefile                    #
#				彭东 	  #
###################################################################
*/
#include "config.h"
MAKEFLAGS = -s

/*x86平台硬件抽象层*/
#ifdef CFG_X86_PLATFORM
KERNELCE_PATH	= ../hal/x86/
LIB_PATH = ../share/lib/

HEADFILE_PATH = -I ../include -I ../include/types -I ../include/halinc -I ../include/knlinc -I ../include/libinc -I ../include/drvinc -I ../share/include -I ../share
#endif

CCBUILDPATH = $(KERNELCE_PATH)

include krnlbuidcmd.mk
include krnlobjs.mk

.PHONY : all everything  build_kernel
all: build_kernel

build_kernel:everything

everything :$(BUILD_MK_HALY_OBJS)
include krnlbuidrule.mk
