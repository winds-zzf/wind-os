/**
 * 键盘驱动程序模块
 * API定义
 */
#ifndef __DRVKEYBOARD_T_H
#define __DRVKEYBOARD_T_H


/* 键盘结构体 */
typedef struct KEYBOARD_T{
	list_t		hook;	//
	spinlock_t	lock;	//保护键盘结构体的自旋锁
	/**
	 * 为什么device_t,driver_t都有了信号量，keyboard_t还要有信号量？？？
	 * device_t描述符不是和设备一一对应吗？？？，这样device_t有了信号量不就足够了？？？4
	 * 键盘结构体的信号量可以实现进程同步功能，确保先输入字符再获取字符的先后顺序
	 */
	sem_t	sem;		//键盘信号量
	uint_t	flag;	//
	uint_t	status;	//
	/**
	 * 为什么要有tail这个字段？？？
	 */
	uint_t	tail;	//
	/**
	 * 键盘缓冲区为什么是u16_t类型，而不是u8_t类型
 	 */
	u16_t	buf[64];	//键盘缓冲区
}keyboard_t;


#define DEV_KEYBOARD_NR (INT_VECTOR_IRQ0+1)		//键盘设备硬件中断号
#define DEV_KEYBOARD_SUBDEV_NR 0				//键盘子设备号
#define DEV_KEYBOARD_NAME "key_board"			//键盘设备名
#define KBBUF_NULL 0xffff					//

#define KEY_SHIFT_L_DOWN 0x2a		//
#define KEY_SHIFT_L_UP 0xaa		//

#define KEY_SHIFT_R_DOWN 0x36		//
#define KEY_SHIFT_R_UP 0xb6		//


#define KB_DATA_REG 0x60			//键盘数据寄存器
#define KB_CMD_REG 0x64			//键盘控制寄存器
#define KB_STATUS_REG 0x64		//键盘状态寄存器

#define READ_CMD_BYTE 0x20		//
#define WRITE_CMD_BYTE 0x60		//

#define SF_CHK 0xaa				//
#define SF_CHK_OK 0x55			//

#define RESET_KB 0xff			//
#define RESET_KB_OK 0xaa			//
#define RESET_KB_FA 0xfc			//
#define KB848SET_DEFOKB 0xf6		//
#define KBCMD 0x29				//

#define SEND_MOUSECMD 0xd4		//

#define KEYBOARD_INTLINE (INT_VECTOR_IRQ0+1)	//键盘设备中断信号线

#define B_K_STUS_O_B_F (1<<0)		//
#define B_K_STUS_I_B_F (1<<1)		//
#define B_K_STUS_S_F (1<<2)		//
#define B_K_STUS_C_D (1<<3)		//
#define B_K_STUS_K_I (1<<4)		//
#define B_K_STUS_T_T (1<<5)		//
#define B_K_STUS_R_T (1<<6)		//
#define B_K_STUS_P_E (1<<7)		//

#define KB_ACK 0xfa				//
#define KB842_OPENIF 0xae		//
#define MOUSE_OPENIF 0xa8		//


#define FGMK 			0x80			/*  */
#define ASCII_MASK 		0x7f			/*  */
#define FLAG_BREAK		0x0080		/* Break Code */
#define FLAG_EXT		0x0100		/* Normal function keys */
#define FLAG_SHIFT_L	0x0200		/* Shift key */
#define FLAG_SHIFT_R	0x0400		/* Shift key */
#define FLAG_CTRL_L		0x0800		/* Control key */
#define FLAG_CTRL_R		0x1000		/* Control key */
#define FLAG_ALT_L		0x2000		/* Alternate key */
#define FLAG_ALT_R		0x4000		/* Alternate key */
#define FLAG_PAD		0x8000		/* keys in num pad */

/**
 * raw key value = code passed to tty & MASK_RAW
 * the value can be found either in the keymap column 0 or in the list below
 */
#define MASK_RAW		0x01FF

/**
 * Special keys
 */
#define ESC			(0x01 + FLAG_EXT)	/* Esc */
#define TAB			(0x02 + FLAG_EXT)	/* Tab */
#define ENTER			(0x03 + FLAG_EXT)	/* Enter */
#define BACKSPACE		(0x04 + FLAG_EXT)	/* BackSpace */

#define GUI_L			(0x05 + FLAG_EXT)	/* L GUI */
#define GUI_R			(0x06 + FLAG_EXT)	/* R GUI */
#define APPS			(0x07 + FLAG_EXT)	/* APPS */

/**
 * Shift, Ctrl, Alt
 */
#define SHIFT_L		(0x08 + FLAG_EXT)	/* L Shift */
#define SHIFT_R		(0x09 + FLAG_EXT)	/* R Shift */
#define CTRL_L			(0x0A + FLAG_EXT)	/* L Ctrl */
#define CTRL_R			(0x0B + FLAG_EXT)	/* R Ctrl */
#define ALT_L			(0x0C + FLAG_EXT)	/* L Alt */
#define ALT_R			(0x0D + FLAG_EXT)	/* R Alt */

/* Lock keys */
#define CAPS_LOCK		(0x0E + FLAG_EXT)	/* Caps Lock */
#define NUM_LOCK		(0x0F + FLAG_EXT)	/* Number Lock */
#define SCROLL_LOCK		(0x10 + FLAG_EXT)	/* Scroll Lock */

/* Function keys */
#define F1			(0x11 + FLAG_EXT)	/* F1 */
#define F2			(0x12 + FLAG_EXT)	/* F2 */
#define F3			(0x13 + FLAG_EXT)	/* F3 */
#define F4			(0x14 + FLAG_EXT)	/* F4 */
#define F5			(0x15 + FLAG_EXT)	/* F5 */
#define F6			(0x16 + FLAG_EXT)	/* F6 */
#define F7			(0x17 + FLAG_EXT)	/* F7 */
#define F8			(0x18 + FLAG_EXT)	/* F8 */
#define F9			(0x19 + FLAG_EXT)	/* F9 */
#define F10			(0x1A + FLAG_EXT)	/* F10 */
#define F11			(0x1B + FLAG_EXT)	/* F11 */
#define F12			(0x1C + FLAG_EXT)	/* F12 */

/* Control Pad */
#define PRINTSCREEN		(0x1D + FLAG_EXT)	/* Print Screen */
#define PAUSEBREAK		(0x1E + FLAG_EXT)	/* Pause/Break */
#define INSERT			(0x1F + FLAG_EXT)	/* Insert */
#define DELETE			(0x20 + FLAG_EXT)	/* Delete */
#define HOME			(0x21 + FLAG_EXT)	/* Home */
#define END        		(0x22 + FLAG_EXT)	/* End */
#define PAGEUP			(0x23 + FLAG_EXT)	/* Page Up */
#define PAGEDOWN		(0x24 + FLAG_EXT)	/* Page Dow */
#define UP			(0x25 + FLAG_EXT)	/* Up */
#define DOWN			(0x26 + FLAG_EXT)	/* Down */
#define LEFT			(0x27 + FLAG_EXT)	/* Left */
#define RIGHT			(0x28 + FLAG_EXT)	/* Right */

/* ACPI keys */
#define POWER			(0x29 + FLAG_EXT)	/* Power */
#define SLEEP			(0x2A + FLAG_EXT)	/* Sleep */
#define WAKE			(0x2B + FLAG_EXT)	/* Wake Up */

/* Num Pad */
#define PAD_SLASH		(0x2C + FLAG_EXT)	/* /	*/
#define PAD_STAR		(0x2D + FLAG_EXT)	/* * */
#define PAD_MINUS		(0x2E + FLAG_EXT)	/* -	*/
#define PAD_PLUS		(0x2F + FLAG_EXT)	/* +	*/
#define PAD_ENTER		(0x30 + FLAG_EXT)	/* Enter */
#define PAD_DOT		(0x31 + FLAG_EXT)	/* .	*/
#define PAD_0			(0x32 + FLAG_EXT)	/* 0	*/
#define PAD_1			(0x33 + FLAG_EXT)	/* 1	*/
#define PAD_2			(0x34 + FLAG_EXT)	/* 2	*/
#define PAD_3			(0x35 + FLAG_EXT)	/* 3	*/
#define PAD_4			(0x36 + FLAG_EXT)	/* 4	*/
#define PAD_5			(0x37 + FLAG_EXT)	/* 5	*/
#define PAD_6			(0x38 + FLAG_EXT)	/* 6	*/
#define PAD_7			(0x39 + FLAG_EXT)	/* 7	*/
#define PAD_8			(0x3A + FLAG_EXT)	/* 8	*/
#define PAD_9			(0x3B + FLAG_EXT)	/* 9	*/
#define PAD_UP			PAD_8		/* Up */
#define PAD_DOWN		PAD_2		/* Down */
#define PAD_LEFT		PAD_4		/* Left */
#define PAD_RIGHT		PAD_6		/* Right */
#define PAD_HOME		PAD_7		/* Home */
#define PAD_END		PAD_1		/* End */
#define PAD_PAGEUP		PAD_9		/* Page Up */
#define PAD_PAGEDOWN	PAD_3		/* Page Down */
#define PAD_INS		PAD_0		/* Ins */
#define PAD_MID		PAD_5		/* Middle key */
#define PAD_DEL		PAD_DOT		/* Del */

#define MAP_COLS		3			/* Number of columns in keymap */
#define NR_SCAN_CODES	0x80			/* Number of scan codes (rows in keymap) */


#endif //__DRVKEYBOARD_T_H
