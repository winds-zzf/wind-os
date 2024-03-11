###################################################################
# APP自动化编译配置文件 Makefile #
# 彭东 #
###################################################################
MAKEFLAGS = -s
KERNELCE_PATH = ../apps/
# 为什么要使用内核的头文件？
HEADFILE_PATH = -I ../include -I ../include/bastypeinc -I ../include/halinc -I ../include/knlinc -I ../include/libinc -I ../include/drvinc
CCBUILDPATH = $(KERNELCE_PATH)
include buildcmd.mk
include buildobjs.mk
APPS := $(addsuffix .app, $(basename $(BUILD_MK_APPS_OBJS)))
.PHONY : all linkapp
all :$(BUILD_MK_APPS_OBJS)
linkapp: $(APPS)
%.app:
[make -f appslink.mk OBJ=$@
include buildrule.mk
