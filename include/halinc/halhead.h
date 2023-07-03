/**
 * hal层头文件
 */
#ifndef __HALHEAD_H
#define __HALHEAD_H

#include"type.h"			//全局基本类型
#include"lib.h"			//通用库文件

#include"vgastr.h"			//文本模式
#include"platform.h"		//平台初始化模块
#include"memory.h"			//内存初始化模块
#include"interrupt.h"		//中断初始化模块
#include"global.h"			//全局变量声明头文件

#endif //__HALHEAD_H