########################
# 主控自动化编译配置文件 #
########################

MAKEFLAGS = -sR

.PHONY : build all clean copy pack
 install

all: clean build 
copy pack install clean backup

# 清除文件
clean:
	cd ./loader/; make clean
	cd ./build/; rm -f *.o *.bin *.i *.krnl *.s *.map *.lib *.btoj *.elf *.lds *.mk *.mki krnlobjs.mh
	cd ./release/; rm -f *.bin *.kif

# 编译源代码
build:
	cd ./tools/pack;make	#编译映像打包工具
	cd ./loader;make		#编译二级引导器
	make -f ./Makefile.x86	#编译内核
	
# 拷贝所需的文件到release文件夹
copy:
	cp ./tools/disk/Makefile ./release/	#拷贝磁盘管理工具
	cp ./tools/pack/pack ./release/	#拷贝最新的打包工具	
	cd ./loader/build/;cp grub.bin loader.bin bios.bin ../../release/
	cp ./build/kernel.bin ./release/

# 将内核所需文件打包成映像
pack:
	cd ./release/;./pack -o Wind.kif -g grub.bin -f loader.bin bios.bin kernel.bin logo.bmp font.fnt

# 将内核映像安装到磁盘boot目录
install:
	cd ./release/;make mount
	cd ./release/;sudo cp ./Wind.kif ./hdisk/boot/
	cd ./release/;make umount

# 对源代码进行备份
backup:
	tar --exclude='./release/hd.img' --exclude='./src.tar.gz' -czvf ./src.tar.gz ./
	mv ./src.tar.gz ../backup/src_$$(date +%Y%m%d_%H%M%S).tar.gz

