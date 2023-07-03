MAKEFLAGS = -s

# 头文件路径
HEADFILE_PATH = ../include/
# 程序路径
KRNLBOOT_PATH = ../boot/
# 构造路径
CCBUILDPATH = $(KRNLBOOT_PATH)

LIB_PATH = ../../share/lib/
INCLUDE_PATH = ../../share/include/

include commands.mh
include objects.mh

.PHONY : all everything  build_kernel

all: build_kernel 

build_kernel:everything

everything : $(GRUB_OBJS) $(LOADER_OBJS) $(BIOS_OBJS)

include rules.mh