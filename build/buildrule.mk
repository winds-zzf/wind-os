#####################################
# 自动化编译配置文件 Makefile #
#####################################
# 回显字符串
Echo_GCC = @echo 'GCC -[M] 正在构建'$<
Echo_ASM = @echo 'ASM -[M] 正在构建'$<
%.krnl : $(CCBUILD_PATH)%.c
	$(CC) $(CFLAGS) -o $@ $<
	$(Echo_GCC)
%.krnl : $(CCBUILD_PATH)%.S
	$(CC) $(CFLAGS) -o $@ $<
	$(Echo_GCC)
%.o : $(CCBUILD_PATH)%.asm
	$(ASM) $(ASMFLGS) -o $@ $<
	$(Echo_ASM)
%.o : $(CCBUILD_PATH)%.c
	$(CC) $(CFLAGS) -o $@ $<
	$(Echo_GCC)
%.o : $(CCBUILD_PATH)%.S
	$(CC) $(CFLAGS) -o $@ $<
	$(Echo_GCC)
