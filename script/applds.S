###################################
#		app 编译脚本文件		#
###################################


MAKEFLAGS = -s


include buildcmd.mk
include buildobjs.mk

OBJ :=
ELF_FILE := $(basename $(OBJ)).elf
LINKOBJS := $(BUILD_MK_LIBS_OBJS) $(basename $(OBJ)).o
APP_FILE := $(basename $(OBJ)).app
APPLDFLAGS = -s -static -T applds.lds -n -Map $(APP_FILE).map

.PHONY : all
all: $(APP_FILE)

%.elf:
[$(LD) $(APPLDFLAGS) -o $@ $(LINKOBJS)

%.app:%.elf
[$(OBJCOPY) $(OJCYFLAGS) $< $@

include buildrule.mk
