/**
 * 显卡文本模式
 * API实现
 */
#include"global.h"
#include"vgastr_t.h"

/**
 * 由于Cursor放在了程序的数据区，而不是特定地址下，因此不能多个模块进行共享
 * 因此从一个模块跳转到另一个模块时，必须重新执行初始化{init_cursor();clear_screen(val);close_cursor();}
 */
static __attribute__((section(".data"))) Cursor cursor;		//定义全局光标变量

void init_cursor(){
	cursor.r = 0;
	cursor.c = 0;
	return;
}

void close_cursor(){
	outb(VGA_REG_INDEX,VGA_REG_IX);
	outb(VGA_REG_DATA,VGA_CLOSE);
	return;
}

void open_cursor(){
	;
	return;
}

/**
 * 设置光标位置
 */
void set_cursor(u32_t r,u32_t c){
	u32_t pos = OFFSET(r,c)/2;	//求出光标偏移量
	cursor.r = r;
	cursor.c = c;
	outb(VGA_REG_INDEX,VGA_REG_CURSOR_R);	//准备x光标寄存器
	outb(VGA_REG_DATA,(pos>>8)&0xff);		//写入x光标值
	outb(VGA_REG_INDEX,VGA_REG_CURSOR_C);	//准备y光标寄存器
	outb(VGA_REG_DATA,pos&0xff);			//写入y光标值
	return;
}
void clear_screen(u16_t val){
	set_cursor(0,0);	//光标设置为0;

	//依次处理每一个字符显存数据(16bits)
	u16_t *p = (u16_t*)VGA_ADR;	//将显存地址转换为u16_t指针，这样每次处理两个byte
	for(uint_t i=0;i<VGA_ROW*VGA_COL;i++){
		p[i] = val;	//Linux默认使用小端模式
	}
	return;
}

void scroll_screen(){
	//获取第二行首元素地址
	u16_t* src = (u16_t*)(VGA_ADR+OFFSET(1,0));
	//将后面的所有行移动到前面来
	memcpy((addr_t)VGA_ADR,(addr_t)src,(VGA_ROW-1)*VGA_COL*2);
	src = (u16_t*)(VGA_ADR+OFFSET(VGA_ROW-1,0));
	
	for(uint_t i=0;i<VGA_COL;i++){
		//使用默认值初始化最后一行
		*src++ = (u16_t)VGA_DEFAULT_VAL; 
	}
	return;
}

void write_string(const char_t* str){
	char_t* addr = NULL;
	while(*str){
		//每次处理一个字符之前定位地址
		addr = (char_t*)(VGA_ADR+OFFSET(cursor.r,cursor.c));
		set_cursor(cursor.r,cursor.c);		//设置光标位置
		switch(*str){	//依次处理每个字符
		case '\n':
			cursor.r++;
			cursor.c=0;
			if(cursor.r >= VGA_ROW){	//超过最后一行
				cursor.r--;
				scroll_screen();	//屏幕滚动
			}
			break;
		case '\r':
			cursor.c = 0;	//回到行首
			break;
		case '\t':
			if(cursor.c==ALIGN(cursor.c,VGA_TAB)){	//如果当前位置已经是制表位
				cursor.c++;
			}
			cursor.c = ALIGN(cursor.c,VGA_TAB);
			if(cursor.c >= VGA_COL){	//如果超出列数，另起一行
				cursor.r++;
				cursor.c=0;
				if(cursor.r >= VGA_ROW){
					cursor.r--;
					scroll_screen();	//屏幕滚动
				}
			}
			break;
		case '\v':
			;
			break;
		case '\b':
			if(cursor.c>0){
				cursor.c--;
			}
			break;
		default:		//普通字符
			*addr = *str;
			cursor.c++;
			if(cursor.c >= VGA_COL){
				cursor.c=0;
				cursor.r++;
				if(cursor.r >= VGA_ROW){
					cursor.r--;
					scroll_screen();
				}
			}
			break;
		}
		str++;		//+1B
	}
	return;
}
