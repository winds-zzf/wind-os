/**
 * 
 */
#ifndef __OBJNODE_H
#define __OBJNODE_H

#include "objnode_t.h"

/**
 * 新建一个objnode_t实例
 */
objnode_t* objnode_new();

/**
 * 释放一个objnode_t实例
 */
bool_t objnode_delete(objnode_t* node);

/**
 * 通过函数参数返回值，获取objnode中的字段值
 */
drvstus_t device_objnode_param(void *objnode,buf_t* buf,u64_t* curloc,u64_t* len,u64_t* ioctrl,u64_t* bufcurloc,size_t* bufsz);

#endif //__OBJNODE_H