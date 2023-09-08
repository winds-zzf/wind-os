#####################################
# 自动化编译配置文件 Makefile #
#####################################
MAKEFLAGS = -s
KERNELCE_PATH =
HEADFILE_PATH =
CCBUILD_PATH = $(KERNELCE_PATH)
include buildcmd.mk
include buildobj.mk
.PHONY : all
all : $(BUILD_MK_FSYS_OBJS)
include buildrule.mk
