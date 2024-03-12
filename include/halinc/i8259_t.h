/**
 * 8259A中断控制器
 * 类型定义
 */
#ifndef __8259A_T_H
#define __8259A_T_H


#define MIOPT 0x20		//主片中断操作控制字。用于控制主片的中断屏蔽和中断优先级。
#define SIOPT 0xA0		//从片...

#define MIOPT1 0x21		//主片中断操作控制字1。用于控制主片的中断屏蔽和中断优先级。
#define SIOPT1 0xA1		//从片...

#define ICW1 0x11		//初始化控制字1。用于初始化中断控制器并设置其工作模式。

#define ICW4 0x01		//初始化控制字4。用于设置中断控制器的额外功能和工作模式。

#define MICW2 0x20		//主片中断控制字2。用于设置主片的中断向量偏移。
#define SICW2 0x28		//从片...

#define MICW3 0x04		//主片中断控制字3。用于设置主片与从片之间的连接方式。
#define SICW3 0x02		//从片...

#define _INTM_CTL 		0x20		// I/O port for interrupt controller         <Master>
#define _INTM_CTLMASK	0x21		// setting bits in this port disables ints   <Master>
#define _INTS_CTL 		0xA0		// I/O port for second interrupt controller  <Slave>
#define _INTS_CTLMASK 	0xA1		// setting bits in this port disables ints   <Slave>
#define _EOI 0x20				// end of interrupt.


#endif //__8259A_T_H