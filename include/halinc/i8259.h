/**
 * 8259A中断控制器
 * API定义
 */
#ifndef __8259A_H
#define __8259A_H

/**
 * 中断的来源除了来自于硬件自身的NMI中断和来自于软件的INT n指令造成的软中断之外
 * 还有来自于外部硬件设备的中断，这些中断是可屏蔽的，这些中断也都可以通过PIC进行控制，并传递给CPU
 */


/**
 * 初始化8259A中断控制器
 */
void i8259_init();

/**
 * 显示8259A控制器信息
 */
void i8259_display();


/**
 * 向8259A发送EOI(End of Interrupt)信号，告知8259a芯片中断处理的结束
 * 随后8259a选择下一次的中断信号
 */
void i8259_send_eoi();

/**
 * 使能指定的中断线
 * line: 中断线
 */
void i8259_enable_line(u32_t line);

/**
 * 禁用指定的中断线
 * line: 中断线
 */
void i8259_disable_line(u32_t line);

/**
 * 保存并禁用指定的中断线
 * svline: 
 * line: 中断线
 */
void i8259_save_disableline(u64_t *svline, u32_t line);

/**
 * 恢复指定中断线的使能状态
 * svline: 
 * line: 中断线
 */
void i8259_rest_enableline(u64_t *svline, u32_t line);


#endif //__8259A_H
