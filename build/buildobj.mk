# 引导器构造目标
BUILD_MK_BOOT_OBJS := BUILD_BOOT_OBJS
# hal层构造目标
BUILD_MK_HALY_OBJS := _hal.o hal.o vgastr.o stdio.o string.o utils.o allocate.o global.o platform.o memory.o i8259.o _isr.o interrupt.o cpuctrl.o gdt.o idt.o tss.o mempage.o memtable.o memarea.o memmgr.o
 # kernel构造目标
BUILD_MK_KRNL_OBJS := kernel.o callback.o
 # 内存管理构造目标
BUILD_MK_MEMY_OBJS :=
 # 文件系统构造目标
BUILD_MK_FSYS_OBJS :=
 # 驱动程序构造目标
BUILD_MK_DRIV_OBJS :=
 # 库构造目标
BUILD_MK_LIBS_OBJS :=
 # 进程管理构造目标
BUILD_MK_TASK_OBJS :=
 # link目标
BUILD_MK_LINK_OBJS := _hal.o hal.o vgastr.o stdio.o string.o utils.o allocate.o global.o platform.o memory.o i8259.o _isr.o interrupt.o cpuctrl.o gdt.o idt.o tss.o mempage.o memtable.o memarea.o memmgr.o kernel.o callback.o
 # elf文件
KERNL_MK_ELFF_FILE := kernel.elf
 # bin文件
KERNL_MK_BINF_FILE := kernel.bin
