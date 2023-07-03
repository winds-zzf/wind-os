/**
 * 显卡文本模式
 * API定义
 */
#ifndef __VGASTR_H
#define __VGASTR_H


/**
 * 初始化光标
 */
void init_cursor();

/**
 * 关闭光标
 */
void close_cursor();

/**
 * 显示光标
 */
void open_cursor();

/**
 * 设置光标位置
 */
void set_cursor(u32_t r,u32_t c);

/**
 * 清空屏幕
 */
void clear_screen(u16_t val);

/**
 * 滚动屏幕
 */
void scroll_screen();

/**
 * 从键盘读取用户输入，并显示在控制台上
 * dst:接收读取的字符串
 */
void read_string(const char_t* str);

/**
 * 屏幕输出函数
 * 纯色背景
 * str:待输出字符串
 */
void write_string(const char_t* str);

/**
 * 向指定端口写一个字节
 * port
:端口号
 * val
:8位值
 */
INLINE void outb(const u16_t port,const u8_t val){
	__asm__ __volatile__("outb %1,%0\n"
		:
		:"dN"(port),"a"(val)
	);
}

/**
 * 从指定端口读一个字节
 * port
:端口号
 */
INLINE u8_t inb(const u16_t port){
    u8_t ret;
    __asm__ __volatile__("inb %1, %0\n"
             : "=a"(ret)
             : "dN"(port)
    );
    return ret;
}


/**
 * 向指定端口写一个字
 * port
:端口号
 * val
:16位值
 */
INLINE void outw(const u16_t port,const u16_t val){
    __asm__ __volatile__("outw  %1, %0\n"
             :
             : "dN"(port), "a"(val)
    );
}

/**
 * 从指定端口读一个字
 * port:端口号
 */
INLINE u16_t inw(const u16_t port){
    u16_t ret;
    __asm__ __volatile__("inw %1, %0\n"
             : "=a"(ret)
             : "dN"(port)
    );
    return ret;
}

#endif //__VGASTR_H