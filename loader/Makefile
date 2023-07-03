########################
# 主控自动化编译配置文件 #
########################

MAKEFLAGS = -sR

BUILD_PATH = ./build

.PHONY : build print clean all

build: clean print all

all:
	make -C $(BUILD_PATH) -f loader.mk		# 编译
	make -C $(BUILD_PATH) -f link.mk		# 链接
	@echo '恭喜我，loader编译构建完成！ ^_^'
	
clean:
	cd $(BUILD_PATH); rm -f *.o *.bin *.i *.s *.map *.lib *.btoj *.kif *.elf
	@echo 'loader:清理全部已构建文件... ^_^'

print:
	@echo '*********正在开始编译构建系统*************'




