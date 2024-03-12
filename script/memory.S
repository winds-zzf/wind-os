###################################
#      自动化编辑脚本 Makefile      #
###################################

MAKEFLAGS = -s

KERNELCE_PATH = 
HEADFILE_PATH = 
CCBUILD_PATH = $(KERNELCE_PATH)

include buildcmd.mk
include buildobj.mk

.PHONY :all
all : $(BUILD_MK_MEMY_OBJS)

include buildrule.mk
