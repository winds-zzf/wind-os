###################################
# hal自动化编译配置文件 Makefile #
###################################
MAKEFLAGS = -s
KERNELCE_PATH = ../hal/x86/
HEADFILE_PATH = -I ../include -I ../include/types -I ../include/halinc -I ../include/knlinc -I ../include/libinc -I ../include/drvinc
CCBUILD_PATH = $(KERNELCE_PATH)
include buildcmd.mk
include buildobj.mk
.PHONY : all
all : $(BUILD_MK_HALY_OBJS)
include buildrule.mk
# KERNELCE_PATH 是该模块所需程序文件(.c\.asm)的路径
# HEADFILE_PATH 是该模块依赖的头文件(.h\.inc)的路径
