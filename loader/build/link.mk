include commands.mh
include objects.mh

.PHONY : all build_elf build_bin

all: build_elf build_bin

# 构造elf模块
build_elf: $(GRUB_ELF) $(LOADER_ELF) $(BIOS_ELF)
# 构造bin模块
build_bin:$(GRUB_BIN) $(LOADER_BIN) $(BIOS_BIN)

# grub.elf <- grub.lds
$(GRUB_ELF): $(GRUB_LINK)
	$(LD) $(LDRIMHLDFLAGS) -o $@ $(GRUB_LINK)
# loader.elf <- loader.lds
$(LOADER_ELF): $(LOADER_LINK)
	$(LD) $(LDRKRLLDFLAGS) -o $@ $(LOADER_LINK)
# bios.elf <- bios.lds
$(BIOS_ELF): $(BIOS_LINK)
	$(LD) $(LDRSVELDFLAGS) -o $@ $(BIOS_LINK)

# grub.bin <- grub.elf
$(GRUB_BIN):$(GRUB_ELF)
	$(OBJCOPY) $(OJCYFLAGS) $< $@
	@echo 'OBJCOPY -[M] 正在构建...' $@  
# loader.bin <- loader.elf
$(LOADER_BIN):$(LOADER_ELF)
	$(OBJCOPY) $(OJCYFLAGS) $< $@
	@echo 'OBJCOPY -[M] 正在构建...' $@ 
# bios.bin <- bios.elf
$(BIOS_BIN):$(BIOS_ELF)
	$(OBJCOPY) $(OJCYFLAGS) $< $@
	@echo 'OBJCOPY -[M] 正在构建...' $@