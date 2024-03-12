MAKEFLAGS = -s

# 头文件路径
HEADFILE_PATH = ../include/
# 程序路径
KRNLBOOT_PATH = ../boot/
# 构造路径
CCBUILDPATH = $(KRNLBOOT_PATH)

include commands.mh
include objects.mh

.PHONY : all
all : $(GRUB_OBJS) $(BIOS_OBJS) $(LOADER_OBJS)

include rules.mh