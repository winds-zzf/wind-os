#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"global.h"		//全局头文件放在最开始
#include"image_t.h"
#include"pack.h"

static void console(int argc, char* argv[]);
static void c_list();
static void c_help();
static void c_add();
static void c_convert();
static void c_remove();
static void c_update();
static void c_copy();
static void c_out();
static void c_version();
static void c_grub();
static void c_file();
static void c_pack();
static void c_unpack();
static void c_other();
static void (*cmd)(void) = c_pack;

char image[FNAME_MAX_LEN];	//目标映像名
char files[100][FNAME_MAX_LEN];		//存处文件名
int size;				//文件数目
int p;					//工作指针

int main(int argc,char* argv[]) {
	if (argc <= 1) {
		printf("please enter 'help' if you want to get detail command\n");
		return 0;
	}
	console(argc,argv);
	cmd();
	return 0;
}

static void console(int argc,char* argv[]) {
	p = 1;
	size = 0;
	while(p < argc) {
		if (strcmp(argv[p], "-o") == 0) {				//out
			p++;
			strcpy(image, argv[p++]);
			c_out();
		}
		else if (strcmp(argv[p], "-f") == 0) {			//source file
			p++;
			for (size = 1; p<argc && '-'!= argv[p][0]; size++) {
				strcpy(files[size], argv[p++]);
			}
			c_file();
		}
		else if (strcmp(argv[p], "-g") == 0) {			//grub header
			p++;
			strcpy(files[0], argv[p++]);
			c_grub();
		}
		else if (strcmp(argv[p], "help") == 0) {		//help
			p++;
			cmd = c_help;
		}
		else if (strcmp(argv[p], "version") == 0) {		//version
			p++;
			cmd = c_version;
		}
		else if (strcmp(argv[p], "list") == 0) {		//list
			p++;
			cmd = c_list;
		}
		else if (strcmp(argv[p], "copy") == 0) {		//copy
			p++;
			cmd = c_copy;
		}
		else if (strcmp(argv[p], "remove") == 0) {		//remove
			p++;
			cmd = c_remove;
		}
		else if (strcmp(argv[p], "add") == 0) {			//add
			p++;
			cmd = c_add;
		}
		else if (strcmp(argv[p], "convert") == 0) {		//convert
			p++;
			cmd = c_convert;
		}
		else if (strcmp(argv[p], "update") == 0) {		//update
			p++;
			cmd = c_update;
		}
		else if (strcmp(argv[p], "unpack") == 0) {		//unpack
			p++;
			strcpy(image, argv[p++]);
			cmd = c_unpack;
		}
		else {
			p++;
			cmd = c_other;
		}
	}
}

static void _pack_() {
	return;
}

static void c_pack() {
	printf("正在打包映像%s...\n",image);
	_pack_();
	pack(image, files, size);
	printf("映像%s打包结束\n",image);
	return;
}

static void _list_() {
	return;
}

/**
 *
 */
static void c_list() {
	_list_();
	return;
}

static void _help_() {
	return;
}

/**
 *
 */
static void c_help() {
	_help_();
	return;
}

static void _add_() {
	return;
}

/**
 *
 */
static void c_add() {
	_add_();
	return;
}

static void _convert_() {
	return;
}
/**
 *
 */
static void c_convert() {
	_convert_();
	return;
}

static void _remove_() {
	return;
}

/**
 *
 */
static void c_remove() {
	_remove_();
	return;
}

static void _update_() {
	return;
}

/**
 *
 */
static void c_update() {
	_update_();
	return;
}


static void _copy_() {
	return;
}

/**
 *
 */
static void c_copy() {
	_copy_();
	return;
}

static void _out_(){
	return;
}
/**
 *
 */
static void c_out() {
	_out_();
	return;
}

static void _version_() {
	return;
}
/**
 *
 */
static void c_version() {
	_version_();
	return;
}

static void _grub_() {
	return;
}
/**
 *
 */
static void c_grub() {
	_grub_();
	return;
}

static void _file_() {
	return;
}
/**
 *
 */
static void c_file() {
	_file_();
	return;
}

static void _unpack_() {
	return;
}
/**
 *
 */
static void c_unpack() {
	printf("正在解包映像%s\n",image);
	_unpack_();
	unpack(image);
	printf("映像解包完成...\n");
	return;
}

static void _other_() {
	return;
}
/**
 *
 */
static void c_other() {
	_other_();
	return;
}