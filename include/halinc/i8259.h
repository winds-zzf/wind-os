/**
 * 8259A中断控制器
 * API定义
 */
#ifndef __8259A_H
#define __8259A_H


/**
 * 初始化8259A中断控制器
 */
void i8259_init();

/**
 * 显示8259A控制器信息
 */
void i8259_display();


/**
 * 向8259A发送EOI(End of Interrupt)信号，告知中断处理的结束
 * 
 */
void i8259_send_eoi();

/**
 * 使能指定的中断线
 * line: 中断线
 */
void i8259_enabled_line(u32_t line);

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
void i8259_rest_enabledline(u64_t *svline, u32_t line);


#endif //__8259A_H
