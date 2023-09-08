########################
#       总控make文件      #
########################
MAKEFLAGS = -s

.PHONY : all clean backup prepare build copy pack install start

BACKUP_FILE := ./hal ./include ./kernel ./loader ./script ./share ./tools ./Makefile ./Makefile.x86

# 清理->备份->构造->复制->打包->安装->启动
all: clean backup prepare build copy pack install start


# 清除：清除生成的目标文件
clean:
	cd ./release/; rm -f *.bin *.kif 	# 清理发布模块
	cd ./build; rm -f *.mk *.lds *.o *.map *.elf *.bin		# 清理构造模块 
	@echo 清理全部已构建kernel文件... ^_^

# 备份：备份源代码
backup:
	tar -czf ../backup/src_$$(date +%Y%m%d_%H%M%S).tar.gz $(BACKUP_FILE)


# 预处理：加载所需的脚本文件，编译工具程序
prepare:
	cd ./tools/pack; make		# 编译映像打包工具
	cd ./tools/script; make		# 编译脚本生成工具
	cp ./tools/script/scripter ./script/ 	# 拷贝最新的脚本生成工具
	cp ./tools/pack/packer ./release/		# 拷贝最新的映像打包工具
	cp ./tools/disk/Makefile ./release/	# 拷贝磁盘管理脚本

# 编译：引导器&内核
build:
	cd ./loader; make			# 编译loader
	make -f ./Makefile.x86		# 编译kernel

	
# 拷贝：拷贝必要的程序和脚本
copy:
	cd ./loader/build/;cp grub.bin loader.bin bios.bin  ../../release/ # 拷贝引导器文件
	cp ./build/kernel.bin ./release/ 		# 拷贝内核文件
	

# 打包：将内核所需文件打包成映像
pack:
	cd ./release/;./packer -o Wind.kif -g grub.bin -f loader.bin bios.bin kernel.bin font.ttf


# 安装：将内核映像安装到磁盘boot目录
install:
	@echo '=========================正在安装Wind.kif映像========================='
	cd ./release/;make mount
	cd ./release;sudo cp ./Wind.kif ./hdisk/boot/
	cd ./release/;make umount
	@echo '=========================Wind.kif映像安装结束========================='


# 启动：启动虚拟机进行测试
start: