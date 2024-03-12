cd c:/Software/VirtualBox/
VBoxManage controlvm OS poweroff
VBoxManage storagectl OS --name "SATA" --remove
del C:\Users\ZZF\Desktop\fsdownload\hd.vdi
VBoxManage convertfromraw C:/Users/ZZF/Desktop/fsdownload/hd.img --format VDI C:/Users/ZZF/Desktop/fsdownload/hd.vdi
VBoxManage storagectl OS --name "SATA" --add sata --controller IntelAhci --portcount 1
VBoxManage closemedium disk C:/Users/ZZF/Desktop/fsdownload/hd.vdi
VBoxManage storageattach OS --storagectl "SATA" --port 1 --device 0 --type hdd --medium C:/Users/ZZF/Desktop/fsdownload/hd.vdi
VBoxManage startvm OS