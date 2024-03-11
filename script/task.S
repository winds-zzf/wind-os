#####################################
#   task自动化编译配置文件 Makefile   #
#####################################

MAKEFLAGS = -s

KERNELCE_PATH	= ../task/
HEADFILE_PATH = -I ../include \
			-I ../include/types \
			-I ../include/halinc \
			-I ../include/knlinc \
			-I ../include/libinc \
			-I ../include/drvinc
			
CCBUILD_PATH	= $(KERNELCE_PATH)

include buildcmd.mk
include buildobj.mk

.PHONY : all
all : $(BUILD_MK_TASK_OBJS)

include buildrule.mk
