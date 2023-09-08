###############################
# 自动化编译配置文件 Makefile #
###############################
MAKEFLAGS = -s
include buildcmd.mk
include buildobj.mk
.PHONY : all
all :
	$(LD) $(LDFLAGS) -o $(KERNL_MK_ELFF_FILE) $(BUILD_MK_LINK_OBJS)
	$(OBJCOPY) $(OJCYFLAGS) $(KERNL_MK_ELFF_FILE) $(KERNL_MK_BINF_FILE)
