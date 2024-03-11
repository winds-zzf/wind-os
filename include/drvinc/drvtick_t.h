/**
 * 时钟硬件驱动
 * 类型定义
 */
#ifndef __DRVTICK_T_H
#define __DRVTICK_T_H


/*  */
#define PTIPROT1 0x40
#define PTIPROT2 0x41
#define PTIPROT3 0x42
#define PTIPROTM 0x43
/*  */
#define TIMEMODE 0x34      	//;00-11-010-0
#define TIMEJISU 1194000UL 	//1193182UL
#define HZ 1000UL          	//0x3e8
/*  */
#define HZLL ((TIMEJISU / HZ) & 0xff)       	 //0xa9//0x9b     //;1001  1011
#define HZHH (((TIMEJISU / HZ) >> 8) & 0xff) 	//0x04//0x2e     //;0010  1110


#endif //__DRVTICK_T_H