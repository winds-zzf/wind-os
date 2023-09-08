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

# CFLGS用于编译.c文件时使用，可以通过(HEADFILE_PATH)指定搜索的头文件(.h)路径
CFLAGS = $(HEADFILE_PATH) -c -O2 -m64 -mtune=generic -mcmodel=large -mno-red-zone -std=c99 -fexec-charset=UTF-8 -Wall -Wshadow  -W -Wconversion -Wno-sign-conversion -fno-stack-protector -fno-zero-initialized-in-bss -fomit-frame-pointer -fno-builtin -fno-common  -fno-ident -ffreestanding  -Wno-unused-parameter -Wunused-variable #-save-temps -fdata-sections -gstabs+

#ASMFLGS用于编译.asm文件时使用，可以通过(HEADFILE_PATH)指定搜索的头文件(.inc)路径
ASMFLGS	= $(HEADFILE_PATH) -f elf64

#CPPFLGSLDS用于预处理.S脚本文件时使用，可以通过(HEADFILE_PATH)指定搜索的头文件路径
CPPFLGSLDS = $(HEADFILE_PATH) -E -P

#LDFLAGS用于链接是使用的命令
LDFLAGS	= -s -static -T windlink.lds -n -Map wind.map

#
OBJCOPY = objcopy

#
OJCYFLAGS = -S -O binary

#endif

