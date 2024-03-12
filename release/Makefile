##################
# 自动化编译程序	  #
#################

MAKEFLAGS = -sR

MAKE = make

# 映像与挂载目录
IMG_NAME = os.img
IMG_SIZE = 204800
IMG_BS = 512
DEVICE = /dev/loop23
ROOT = hdisk

# GRUB相关文件
GRUB_CFG = grub.cfg
GRUB_BAK = grub.cfg.bak
GRUB_HOME = $(ROOT)/boot/grub

# 伪指令
.PHONY: help create mount umount install append update recovery backup

help:
	@echo '<help>		帮助'
	@echo '<create>	创建一个img磁盘，并格式化为ext文件系统'
	@echo '<install>	安装GRUB，并生成grub.cfg文件'
	@echo '<mount>		挂载磁盘文件到hdisk'
	@echo '<umount>	解除hdisk上的挂载'
	@echo '<append>	向grub.cfg中追加代码'
	@echo '<update>	更新grub.cfg'
	@echo '<recovery>	恢复grub.cfg的备份文件'
	@echo '<backup>	对grub.cfg进行备份'

# 创建磁盘、格式化磁盘
create:
	dd bs=$(IMG_BS) if=/dev/zero of=./$(IMG_NAME) count=$(IMG_SIZE)
	sudo losetup $(DEVICE) ./$(IMG_NAME) # 绑定到loop设备上
	sudo mkfs.ext4 -q $(DEVICE)	# 格式化磁盘
	sudo losetup -d $(DEVICE)	# 解除设备循环绑定

# 挂载磁盘
mount:
	sudo losetup $(DEVICE) ./$(IMG_NAME) # 设置为loop设备
	-mkdir $(ROOT)	#创建hdisk文件夹
	sudo mount -o loop ./$(IMG_NAME) ./$(ROOT)/	# 挂载到指定目录

# 解除挂载
umount:
	sudo umount $(DEVICE)		#解除挂载
	sudo rmdir ./$(ROOT)
	sudo losetup -d $(DEVICE)	# 解除设备循环绑定

# 安装GRUB
install: 
	$(MAKE) mount
	sudo grub-install --boot-directory=./$(ROOT)/boot/ --force --allow-floppy $(DEVICE)
	sudo touch ./$(GRUB_HOME)/$(GRUB_CFG)
	$(MAKE) umount

# 追加配置
append : 
	$(MAKE) mount
	sudo cp -f $(GRUB_HOME)/$(GRUB_CFG) ./$(GRUB_BAK) # 备份旧的配置文件，并覆盖已有的备份文件
	sudo cat ./$(GRUB_CFG) >> $(GRUB_HOME)/$(GRUB_CFG)	# 将要追加的字符串追加进去
	$(MAKE) umount

# 替换配置
update :
	$(MAKE) mount
	sudo mv -f $(GRUB_HOME)/$(GRUB_CFG) ./$(GRUB_BAK) # 备份旧的配置文件,并覆盖已有的备份文件
	sudo cp ./$(GRUB_CFG) $(GRUB_HOME)/$(GRUB_CFG)	# 添加新的配置文件
	$(MAKE) umount

# 恢复备份
recovery:
	$(MAKE) mount
	sudo cp -f $(GRUB_BAK) $(GRUB_HOME)/$(GRUB_CFG) # 恢复备份配置文件
	$(MAKE) umount

# 创建备份
backup:
	$(MAKE) mount
	sudo cp -f $(GRUB_HOME)/$(GRUB_CFG) ./$(GRUB_BAK)	#创建备份
	$(MAKE) umount
	
