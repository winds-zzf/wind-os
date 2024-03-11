/**
 * 文本模式
 * 类型定义
 */
#ifndef __VGASTR_T_H
#define __VGASTR_T_H


#define VGA_ADR	0xB8000U		//显存地址
#define VGA_END	0xBffffU		//显存结束地址
#define VGA_ROW	25U			    //默认最大行数25
#define VGA_COL 80U			    //默认最大列数80
#define OFFSET(r,c) (((c)+(r)*VGA_COL)*2) //屏幕(r,c)处字符的偏移地址 
#define VGA_DEFAULT_VAL 0x0f00U	//默认字符
#define VGA_TAB  4              //制表符位宽

//显卡寄存器访问端口
#define VGA_REG_INDEX 0x3d4	//内部寄存器索引
#define VGA_REG_DATA 0x3d5	//内部寄存器数据
//
#define VGA_REG_IX 0x0a
#define VGA_CLOSE 0x20
//光标寄存器索引
#define VGA_REG_CURSOR_R 0x0e		//x光标寄存器索引
#define VGA_REG_CURSOR_C 0x0f		//y光标寄存器索引

//光标结构体
typedef struct CURSOR{
	u32_t r;	//行索引
	u32_t c;	//列索引
}Cursor;


#endif //___VGASTR_H

/**
 *  参数表											
 * --------------------------
 * | R G B | 颜色  	| 浅色	|								 
 * |-------|--------|-------|
 * | 0 0 0 | 黑		| 灰		|
 * | 0 0 1 | 蓝 		| 浅蓝	|
 * | 0 1 0 | 绿		| 浅绿	|
 * | 0 1 1 | 青		| 浅青	|
 * | 1 0 0 | 品红	| 浅红	|
 * | 1 0 1 | 品红	| 浅品红	|
 * | 1 1 0 | 棕		| 黄		|
 * | 1 1 1 | 白		| 亮白	|
 * --------------------------
 */