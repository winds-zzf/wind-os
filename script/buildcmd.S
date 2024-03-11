#####################################
#  自动化编译配置文件 Makefile  #
#####################################

#include "config.h"
#ifdef CFG_X86_PLATFORM

ASM 	= nasm
CC	= gcc
LD	= ld
DD	= dd
RM	= rm

#
OYFLAGS = -O binary

# CFLAGS用于编译.c文件时使用，可以通过(HEADFILE_PATH)指定搜索的头文件(.h)路径
CFLAGS = $(HEADFILE_PATH) -c -O2 -m64 -mtune=generic -mcmodel=large -mno-red-zone -std=c99 -fexec-charset=UTF-8 -Wall -Wshadow  -W -Wconversion -Wno-sign-conversion -fno-stack-protector -fno-zero-initialized-in-bss -fomit-frame-pointer -fno-builtin -fno-common  -fno-ident -ffreestanding  -Wno-unused-parameter -Wunused-variable #-save-temps -fdata-sections -gstabs+

#ASMFLGS用于编译.asm文件时使用，可以通过(HEADFILE_PATH)指定搜索的头文件(.inc)路径
ASMFLGS = $(HEADFILE_PATH) -f elf64

#CPPFLGSLDS用于预处理.S脚本文件时使用，可以通过(HEADFILE_PATH)指定搜索的头文件路径
CPPFLGSLDS = $(HEADFILE_PATH) -E -P

#LDFLAGS用于链接是使用的命令
LDFLAGS = -s -static -T windlink.lds -n -Map wind.map

#
OBJCOPY = objcopy

#
OJCYFLAGS = -S -O binary

#endif


/**
-s：
	用于在链接过程中剥离符号表，减小生成的可执行文件的大小。
-static：
	指定链接器使用静态链接方式，将所有依赖的库都包含在生成的可执行文件中。
-T windlink.lds：
	指定链接器使用名为"windlink.lds"的链接脚本文件，该脚本文件定义了链接过程中的内存布局和其他链接细节。
-n：
	指定链接器在链接过程中显示详细的链接信息。
-Map wind.map：
	指定链接器在链接过程中生成一个名为"wind.map"的映射文件，该文件包含了生成的可执行文件中各个符号的地址和大小等信息。
*/