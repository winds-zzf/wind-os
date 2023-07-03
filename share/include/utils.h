/**
 * 工具模块
 * API定义
 */
#ifndef __UTILS_H
#define __UTILS_H


#include"lib.h"
#include"utils_t.h"

/**
 * utf-8转为unicode，使用long以保证支持UCS2和UCS4
 * utf8:utf8编码值
 */
u64_t utf_to_unicode(const u64_t utf8);

/**
 * unicode转为utf-8
 * unicode:unicode编码值
 */
u64_t unicode_to_utf8(const u64_t unicode);

/**
 * 延时函数
 * delay:dt=0表示永久延时，dt>0表示延时一段时间
 */
void die(uint_t dt);

/**
 * 内存数据8位CRC校验和
 * addr:起始地址
 * size:大小
 * poly:crc多项式
 */
u8_t checksum_crc8(addr_t addr,size_t size,u8_t poly);

/**
 * 内存数据16位CRC校验和
 * addr:起始地址
 * size:大小
 * poly:crc多项式
 */
u16_t checksum_crc16(addr_t addr,size_t size,u16_t poly);

/**
 * 内存数据32位CRC校验和
 * addr:起始地址
 * size:大小
 * poly:crc多项式
 */
u32_t checksum_crc32(addr_t addr,size_t size,u32_t poly);

/**
 * 内存数据64位CRC校验和
 * addr:起始地址
 * size:大小
 * poly:crc多项式
 */
u64_t checksum_crc64(addr_t addr,size_t size,u64_t poly);


#endif //__UTILS_H