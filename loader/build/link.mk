#############################
#                           #
#############################
MAKEFLAGS = -s
Echo_Linker = @echo 'LINKER -[M] 正在链接'$@
Echo_Objcopy = @echo 'OBJCOPY -[M] 正在构建'$@

include commands.mh
include objects.mh

.PHONY : all build_elf build_bin

all: build_elf build_bin

# 构造elf模块
build_elf: $(GRUB_ELF) $(LOADER_ELF) $(BIOS_ELF)
# grub.elf <- grub.lds
$(GRUB_ELF): $(GRUB_LINK)
	$(LD) $(LDGRUBFLAGS) -o $@ $(GRUB_LINK)
	$(Echo_Linker)
# loader.elf <- loader.lds
$(LOADER_ELF): $(LOADER_LINK)
	$(LD) $(LDLOADERFLAGS) -o $@ $(LOADER_LINK)
	$(Echo_Linker)
# bios.elf <- bios.lds
$(BIOS_ELF): $(BIOS_LINK)
	$(LD) $(LDBIOSFLAGS) -o $@ $(BIOS_LINK)
	$(Echo_Linker)
	

# 构造bin模块
build_bin:$(GRUB_BIN) $(LOADER_BIN) $(BIOS_BIN)
# grub.bin <- grub.elf
$(GRUB_BIN):$(GRUB_ELF)
	$(OBJCOPY) $(OBJFLAGS) $< $@
	$(Echo_Objcopy)
# loader.bin <- loader.elf
$(LOADER_BIN):$(LOADER_ELF)
	$(OBJCOPY) $(OBJFLAGS) $< $@
	$(Echo_Objcopy)
# bios.bin <- bios.elf
$(BIOS_BIN):$(BIOS_ELF)
	$(OBJCOPY) $(OBJFLAGS) $< $@
	$(Echo_Objcopy)