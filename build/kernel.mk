#####################################
# kernel自动化编译配置文件 Makefile #
#####################################
MAKEFLAGS = -s
SHARE_PATH = ../share/lib/
SHARE_PATH1 = ../hal/x86/
KERNELCE_PATH = ../kernel/
HEADFILE_PATH = -I ../include -I ../include/types -I ../include/halinc -I ../include/knlinc -I ../include/libinc -I ../include/drvinc -I ../share -I ../share/include
CCBUILD_PATH = $(KERNELCE_PATH)
include buildcmd.mk
include buildobj.mk
.PHONY : all
all : $(BUILD_MK_KRNL_OBJS)
include buildrule.mk
