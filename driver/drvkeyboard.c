/**
 * 键盘驱动程序模块
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"


/**
 * 当按下一键时，产生mark码，产生一次IRQ1中断；放开键时，产生break码，产生一次IRQ1中断
 * 因此，当按下A键放开，实际上产生了两次IRQ1中断
 * break码是mark最高位bit7置1得来，即break=mark+0x80
 * scan-code集
 * set0:第一套扫描码集
 * set1:第二套扫描码集
 * set2:第三套扫描码集
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */











/**
 * 键盘状态变量
 * 用于表示按键的按下获释放状态
 */
static u8_t kbd_dowup_stat = 0;


/**
 * keymap:键位映射
 * scan-code:扫描码
 * !shift:没按下shift
 * shift:按下shift
 * E0 XX:
 * PAD:PAD是指小键盘(Numeric Keypad)的按键。PAD是即pad
 */
static u16_t keymap[NR_SCAN_CODES * MAP_COLS] = {
/* =====scan-code====|===!Shift===|===Shift===|===E0 XX===*/
/* 0x00 - none		*/	0,			0,			0,
/* 0x01 - ESC		*/	ESC,			ESC,			0,
/* 0x02 - '1'		*/	'1',			'!',			0,
/* 0x03 - '2'		*/	'2',			'@',			0,
/* 0x04 - '3'		*/	'3',			'#',			0,
/* 0x05 - '4'		*/	'4',			'$',			0,
/* 0x06 - '5'		*/	'5',			'%',			0,
/* 0x07 - '6'		*/	'6',			'^',			0,
/* 0x08 - '7'		*/	'7',			'&',			0,
/* 0x09 - '8'		*/	'8',			'*',			0,
/* 0x0A - '9'		*/	'9',			'(',			0,
/* 0x0B - '0'		*/	'0',			')',			0,
/* 0x0C - '-'		*/	'-',			'_',			0,
/* 0x0D - '='		*/	'=',			'+',			0,
/* 0x0E - BS		*/	BACKSPACE,	BACKSPACE,	0,
/* 0x0F - TAB		*/	TAB,			TAB,			0,
/* 0x10 - 'q'		*/	'q',			'Q',			0,
/* 0x11 - 'w'		*/	'w',			'W',			0,
/* 0x12 - 'e'		*/	'e',			'E',			0,
/* 0x13 - 'r'		*/	'r',			'R',			0,
/* 0x14 - 't'		*/	't',			'T',			0,
/* 0x15 - 'y'		*/	'y',			'Y',			0,
/* 0x16 - 'u'		*/	'u',			'U',			0,
/* 0x17 - 'i'		*/	'i',			'I',			0,
/* 0x18 - 'o'		*/	'o',			'O',			0,
/* 0x19 - 'p'		*/	'p',			'P',			0,
/* 0x1A - '['		*/	'[',			'{',			0,
/* 0x1B - ']'		*/	']',			'}',			0,
/* 0x1C - CR/LF	*/	ENTER,		ENTER,		PAD_ENTER,
/* 0x1D - l.Ctrl	*/	CTRL_L,		CTRL_L,		CTRL_R,
/* 0x1E - 'a'		*/	'a',			'A',			0,
/* 0x1F - 's'		*/	's',			'S',			0,
/* 0x20 - 'd'		*/	'd',			'D',			0,
/* 0x21 - 'f'		*/	'f',			'F',			0,
/* 0x22 - 'g'		*/	'g',			'G',			0,
/* 0x23 - 'h'		*/	'h',			'H',			0,
/* 0x24 - 'j'		*/	'j',			'J',			0,
/* 0x25 - 'k'		*/	'k',			'K',			0,
/* 0x26 - 'l'		*/	'l',			'L',			0,
/* 0x27 - ';'		*/	';',			':',			0,
/* 0x28 - '''		*/	'\'',		'"',			0,
/* 0x29 - '`'		*/	'`',			'~',			0,
/* 0x2A - l.SHIFT	*/	SHIFT_L,		SHIFT_L,		0,
/* 0x2B - '\'		*/	'\\',		'|',			0,
/* 0x2C - 'z'		*/	'z',			'Z',			0,
/* 0x2D - 'x'		*/	'x',			'X',			0,
/* 0x2E - 'c'		*/	'c',			'C',			0,
/* 0x2F - 'v'		*/	'v',			'V',			0,
/* 0x30 - 'b'		*/	'b',			'B',			0,
/* 0x31 - 'n'		*/	'n',			'N',			0,
/* 0x32 - 'm'		*/	'm',			'M',			0,
/* 0x33 - ','		*/	',',			'<',			0,
/* 0x34 - '.'		*/	'.',			'>',			0,
/* 0x35 - '/'		*/	'/',			'?',			PAD_SLASH,
/* 0x36 - r.SHIFT	*/	SHIFT_R,		SHIFT_R,		0,
/* 0x37 - '*'		*/	'*',			'*',    		0,
/* 0x38 - ALT		*/	ALT_L,		ALT_L,  		ALT_R,
/* 0x39 - ' '		*/	' ',			' ',			0,
/* 0x3A - CapsLock	*/	CAPS_LOCK,	CAPS_LOCK,	0,
/* 0x3B - F1		*/	F1,			F1,			0,
/* 0x3C - F2		*/	F2,			F2,			0,
/* 0x3D - F3		*/	F3,			F3,			0,
/* 0x3E - F4		*/	F4,			F4,			0,
/* 0x3F - F5		*/	F5,			F5,			0,
/* 0x40 - F6		*/	F6,			F6,			0,
/* 0x41 - F7		*/	F7,			F7,			0,
/* 0x42 - F8		*/	F8,			F8,			0,
/* 0x43 - F9		*/	F9,			F9,			0,
/* 0x44 - F10		*/	F10,			F10,			0,
/* 0x45 - NumLock	*/	NUM_LOCK,		NUM_LOCK,		0,
/* 0x46 - ScrLock	*/	SCROLL_LOCK,	SCROLL_LOCK,	0,
/* 0x47 - Home		*/	PAD_HOME,		'7',			HOME,
/* 0x48 - CurUp	*/	PAD_UP,		'8',			UP,
/* 0x49 - PgUp		*/	PAD_PAGEUP,	'9',			PAGEUP,
/* 0x4A - '-'		*/	PAD_MINUS,	'-',			0,
/* 0x4B - Left		*/	PAD_LEFT,		'4',			LEFT,
/* 0x4C - MID		*/	PAD_MID,		'5',			0,
/* 0x4D - Right	*/	PAD_RIGHT,	'6',			RIGHT,
/* 0x4E - '+'		*/	PAD_PLUS,		'+',			0,
/* 0x4F - End		*/	PAD_END,		'1',			END,
/* 0x50 - Down		*/	PAD_DOWN,		'2',			DOWN,
/* 0x51 - PgDown	*/	PAD_PAGEDOWN,	'3',			PAGEDOWN,
/* 0x52 - Insert	*/	PAD_INS,		'0',			INSERT,
/* 0x53 - Delete	*/	PAD_DOT,		'.',			DELETE,
/* 0x54 - Enter	*/	0,			0,			0,
/* 0x55 - ???		*/	0,			0,			0,
/* 0x56 - ???		*/	0,			0,			0,
/* 0x57 - F11		*/	F11,			F11,			0,	
/* 0x58 - F12		*/	F12,			F12,			0,	
/* 0x59 - ???		*/	0,			0,			0,	
/* 0x5A - ???		*/	0,			0,			0,	
/* 0x5B - ???		*/	0,			0,			GUI_L,	
/* 0x5C - ???		*/	0,			0,			GUI_R,	
/* 0x5D - ???		*/	0,			0,			APPS,	
/* 0x5E - ???		*/	0,			0,			0,	
/* 0x5F - ???		*/	0,			0,			0,
/* 0x60 - ???		*/	0,			0,			0,
/* 0x61 - ???		*/	0,			0,			0,	
/* 0x62 - ???		*/	0,			0,			0,	
/* 0x63 - ???		*/	0,			0,			0,	
/* 0x64 - ???		*/	0,			0,			0,	
/* 0x65 - ???		*/	0,			0,			0,	
/* 0x66 - ???		*/	0,			0,			0,	
/* 0x67 - ???		*/	0,			0,			0,	
/* 0x68 - ???		*/	0,			0,			0,	
/* 0x69 - ???		*/	0,			0,			0,	
/* 0x6A - ???		*/	0,			0,			0,	
/* 0x6B - ???		*/	0,			0,			0,	
/* 0x6C - ???		*/	0,			0,			0,	
/* 0x6D - ???		*/	0,			0,			0,	
/* 0x6E - ???		*/	0,			0,			0,	
/* 0x6F - ???		*/	0,			0,			0,	
/* 0x70 - ???		*/	0,			0,			0,	
/* 0x71 - ???		*/	0,			0,			0,	
/* 0x72 - ???		*/	0,			0,			0,	
/* 0x73 - ???		*/	0,			0,			0,	
/* 0x74 - ???		*/	0,			0,			0,	
/* 0x75 - ???		*/	0,			0,			0,	
/* 0x76 - ???		*/	0,			0,			0,	
/* 0x77 - ???		*/	0,			0,			0,	
/* 0x78 - ???		*/	0,			0,			0,	
/* 0x78 - ???		*/	0,			0,			0,	
/* 0x7A - ???		*/	0,			0,			0,	
/* 0x7B - ???		*/	0,			0,			0,	
/* 0x7C - ???		*/	0,			0,			0,	
/* 0x7D - ???		*/	0,			0,			0,	
/* 0x7E - ???		*/	0,			0,			0,
/* 0x7F - ???		*/	0,			0,			0
};

/**
 * 初始化键盘结构体
 */
static void keyboard_t_init(keyboard_t* keyboard){
	list_t_init(&keyboard->hook);
	spinlock_init(&keyboard->lock);
	sem_t_init(&keyboard->sem);
	/**
	 * 信号量的值初始化为0是为了实现进程同步功能
	 * 因为“中断将字符写入缓冲区”和“进程从缓冲区读取字符”是有先后循序的
	 * 类似于生产者消费者之间的关系
	 */
	sem_set(&keyboard->sem,SEM_FLAG_MUTEX,0);
	keyboard->flag = 0;
	keyboard->status = 0;
	keyboard->tail = 0;
	memset(keyboard->buf,0,sizeof(keyboard->buf[64]));
	
	return ;
}

/**
 * 读取键盘控制器的数据寄存器，获取扫描码
 */
static u8_t read_keybd(){
	cpuflag_t cpuflag;
	save_flags_cli(&cpuflag);

	//从数据寄存器端口读取8字节数据
	u8_t scan = in_u8(KB_DATA_REG);

	restore_flags_sti(&cpuflag);
	return scan;
}

/**
 * 获取映射码
 */
static u16_t kbd_map_code(u16_t scan,u16_t cols){
	return keymap[scan*cols];
}


/**
 * 将扫描码写入键盘缓冲区
 */
static void kbd_write_scan(keyboard_t* kboard,u16_t scan){
    /**
     * 最高位用以区分按键的按下或释放状态
     */
    if(0 == (scan & (1 << 7))){	//scan最高位为0，按键按下
        kboard->buf[0] = scan;
        kboard->buf[1] = 0;
        kboard->buf[2] = kbd_map_code(scan, 3);
        kbd_dowup_stat = 1;
    }
    //scan最高位为1
    if(0x80 == (scan & (1 << 7))){	//scan最高位为1，键盘松开
        kboard->buf[0] = 0;
        kboard->buf[1] = scan;
        kboard->buf[2] = kbd_map_code(scan & 0x7f, 3);
        kbd_dowup_stat = 2;
    }
    return;
}



drvstus_t keyboard_open(device_t* devp,void* iopack){
	return device_inc_devcount(devp);
}

drvstus_t keyboard_close(device_t* devp,void* iopack){
	return device_dec_devcount(devp);
}

drvstus_t keyboard_read(device_t* devp,void* iopack){
	u64_t sz = 6;		//默认从键盘缓冲区读取6字节
	keyboard_t* kboard = (keyboard_t*)devp->extdata;	//从设备拓展数据中获取键盘管理结构体

	//从iopack中获取需要的参数：buf&len
	buf_t buf;
	u64_t len;
	if(device_objnode_param(iopack,&buf,NULL,&len,NULL,NULL,NULL)==DFCERRSTUS){
		return DFCERRSTUS;
	}

	/**
	 * 进程同步：P原语
	 */
	sem_down(&kboard->sem);

	//sz = min{sz,len}
	sz = sz<len? sz:len;
	
	//从键盘缓冲区拷贝sz大小的数据到objnode缓冲区
	memcpy(buf,kboard->buf,sz);
	return DFCERRSTUS;
}

drvstus_t keyboard_write(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t keyboard_lseek(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t keyboard_ioctrl(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t keyboard_dev_start(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t keyboard_dev_stop(device_t* devp,void* iopack){
	return device_dec_devcount(devp);
}

drvstus_t keyboard_set_powerstus(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t keyboard_enum_dev(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t keyboard_flush(device_t* devp,void* iopack){
	return DFCERRSTUS;
}

drvstus_t keyboard_shutdown(device_t* devp,void* iopack){
	return DFCERRSTUS;
}



static drvstus_t set_driver(driver_t* drvp){
	//驱动接口函数表
	drvp->dipfun[IOIF_CODE_OPEN] = keyboard_open;
	drvp->dipfun[IOIF_CODE_CLOSE] = keyboard_close;
	drvp->dipfun[IOIF_CODE_READ] = keyboard_read;
	drvp->dipfun[IOIF_CODE_WRITE] = keyboard_write;
	drvp->dipfun[IOIF_CODE_LSEEK] = keyboard_lseek;
	drvp->dipfun[IOIF_CODE_IOCTRL] = keyboard_ioctrl;
	drvp->dipfun[IOIF_CODE_DEV_START] = keyboard_dev_start;
	drvp->dipfun[IOIF_CODE_DEV_STOP] = keyboard_dev_stop;
	drvp->dipfun[IOIF_CODE_SET_POWERSTUS] = keyboard_set_powerstus;
	drvp->dipfun[IOIF_CODE_ENUM_DEV] = keyboard_enum_dev;
	drvp->dipfun[IOIF_CODE_FLUSH] = keyboard_flush;
	drvp->dipfun[IOIF_CODE_SHUTDOWN] = keyboard_shutdown;
	//驱动程序名
	drvp->name = "uartdrv";
	return DFCERRSTUS;
}

static drvstus_t set_device(device_t* devp,driver_t* drvp){
	devp->flag = DEVFLAG_SHARE;		//共享设备
	devp->status = DEVSTUS_NORML;		//正常设备
	devp->id.mtype = UART_DEVICE;		//设备主类型
	devp->id.stype = 0;				//设备子类型
	devp->id.num = 0;				//设备号
	//设备文件名
	devp->name = "uart0";
	return DFCERRSTUS;
}

/**
 * 用户按键，触发键盘对应的21号硬件中断，进入中断回调函数
 * 用户每次按键，都会触发一次该中断，然后执行一次字符读取
 */
drvstus_t keyboard_handle(uint_t intnum,device_t* devp,void* sframe){
	keyboard_t* kboard = (keyboard_t*)devp->extdata;		//键盘管理结构存储在设备拓展字段中 
	if(NULL==kboard){
		return DFCERRSTUS;
	}

	//从键盘设备读取一个字符
	u8_t scan = read_keybd();
	printk("%d\n",scan);
	//将刚读入的字符存入键盘缓冲区中
	kbd_write_scan(kboard,scan);
	
	/**
	 * 进程同步：V原语
	 */
	sem_up(&kboard->sem);
	
	return DFCOKSTUS;
}

/**
 * 8042设备初始化
 */
static void init_i8042(){
	/**
	 * 
	 */
	return;
}

/**
 * 初始化键盘数据
 */
static void init_keyboard(device_t* devp){
	//申请键盘结构实例
	void* keyboard = (void*)mempool_new(sizeof(keyboard_t));
	if(NULL==keyboard){
		 printk("initialize keyboard error\n");
		 return;
	}
	keyboard_t_init(keyboard);
	
	//将键盘管理数据结构放在设备的拓展数据字段
	devp->extdata = keyboard;

	/**
	 * 为什么还要初始化i8042？？？
	 */
	init_i8042();
	
	return;
}

drvstus_t drvkeyboard_entry(driver_t* drvp,u64_t val,void* p){
	if(NULL==drvp){
		return DFCERRSTUS;
	}

	//申请一个设备
	device_t *devp = device_new();
	if(NULL==devp){
		return DFCERRSTUS;
	}

	//配置设备和驱动程序
	set_driver(drvp);
	set_device(devp,drvp);

	//将设备添加到驱动程序
	if(device_add_driver(devp,drvp)==DFCERRSTUS){
		if(device_delete(devp)==DFCERRSTUS){	//释放资源
			return DFCERRSTUS;
		}
		return DFCERRSTUS;
	}

	//向内核注册设备
	if(device_register(devp)==DFCERRSTUS){
		if(device_delete(devp)==DFCERRSTUS){	//释放资源
			return DFCERRSTUS;		
		}
		return DFCERRSTUS;
	}

	//安装中断回调函数
	if(install_callback(devp,keyboard_handle,0x21)==DFCERRSTUS){
		return DFCERRSTUS;
	}

	//初始化键盘设备设备
	init_keyboard(devp);

	//开启硬件中断信号线
	if(hal_enable_intline(1)==DFCERRSTUS){
		return DFCERRSTUS;
	}

	return DFCOKSTUS;
}

drvstus_t drvkeyboard_exit(driver_t* drvp,u64_t val,void* p){


	
	return DFCERRSTUS;
}



