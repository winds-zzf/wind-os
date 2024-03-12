/**
 * 内存区
 * API定义
 */

#ifndef __MEMAREA_H
#define __MEMAREA_H


/**
 * 
 */
void memarea_init();

/**
 * marking pages by area type and then mount them into area
 */
void mount_pages();


#endif //__MEMAREA_H
