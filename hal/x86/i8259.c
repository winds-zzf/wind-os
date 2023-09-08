/**
 * 8259A中断控制器
 * API实现
 */
#include "globaltype.h"
#include "globalctrl.h"

void i8259_init(){
	//初始化主从8259a
	out_u8_p(MIOPT,ICW1);	//初始化主片工作模式
	out_u8_p(SIOPT,ICW1);
	out_u8_p(MIOPT1,MICW2);	//设置主片的中断向量偏移
	out_u8_p(SIOPT1,SICW2);
	out_u8_p(MIOPT1,MICW3);	//设置主从片之间的连接方式
	out_u8_p(SIOPT1,SICW3);
	out_u8_p(MIOPT1,ICW4);	//设置中断控制器的额外功能
	out_u8_p(SIOPT1,ICW4);
	//屏蔽全部中断源(0xff)
	out_u8_p(MIOPT1,0xff);
	out_u8_p(SIOPT1,0xff);
	return;
}

void i8259_display(){
	
	return;
}

void i8259_send_eoi(){
	out_u8_p(_INTM_CTL, _EOI);
	out_u8_p(_INTS_CTL, _EOI);
	return;
}


void i8259_enabled_line(u32_t line){
	cpuflag_t flags;
	save_flags_cli(&flags);
	if (line < 8){
		u8_t amtmp = in_u8(_INTM_CTLMASK);
		amtmp &= (u8_t)(~(1 << line));
		out_u8_p(_INTM_CTLMASK, amtmp);
	}
	else{
		u8_t astmp = in_u8(_INTM_CTLMASK);
		astmp &= (u8_t)(~(1 << 2));
		out_u8_p(_INTM_CTLMASK, astmp);
		astmp = in_u8(_INTS_CTLMASK);
		astmp &= (u8_t)(~(1 << (line - 8)));
		out_u8_p(_INTS_CTLMASK, astmp);
	}
	restore_flags_sti(&flags);
	return;
}

void i8259_disable_line(u32_t line){
	cpuflag_t flags;
	save_flags_cli(&flags);
	if (line < 8){
		u8_t amtmp = in_u8(_INTM_CTLMASK);
		amtmp |= (u8_t)((1 << line));
		out_u8_p(_INTM_CTLMASK, amtmp);
	}
	else{
		u8_t astmp = in_u8(_INTM_CTLMASK);
		astmp |= (u8_t)((1 << 2));
		out_u8_p(_INTM_CTLMASK, astmp);
		astmp = in_u8(_INTS_CTLMASK);
		astmp |= (u8_t)((1 << (line - 8)));
		out_u8_p(_INTS_CTLMASK, astmp);
	}
	restore_flags_sti(&flags);
	return;
}

void i8259_save_disableline(u64_t *svline, u32_t line){
	u32_t intftmp;
	cpuflag_t flags;
	save_flags_cli(&flags);
	u8_t altmp = in_u8(_INTM_CTLMASK);
	intftmp = altmp;
	altmp = in_u8(_INTS_CTLMASK);
	intftmp = (intftmp << 8) | altmp;
	*svline = intftmp;
	i8259_disable_line(line);

	restore_flags_sti(&flags);
	return;
}

void i8259_rest_enabledline(u64_t *svline, u32_t line){
	cpuflag_t flags;
	save_flags_cli(&flags);

	u32_t intftmp = (u32_t)(*svline);

	u8_t altmp = (intftmp & 0xff);
	out_u8_p(_INTS_CTLMASK, altmp);
	altmp = (u8_t)(intftmp >> 8);
	out_u8_p(_INTM_CTLMASK, altmp);

	restore_flags_sti(&flags);

	return;
}

