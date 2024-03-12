/**
 *
 */
#ifndef __KVMBOX_H
#define __KVMBOX_H


/**
 * 初始化KvmBox
 */
void kvmBox_init(KvmBox *box);


/**
 * 初始化KvmBoxs
 */
void kvmBoxs_init(KvmBoxs *boxs);


/**
 * 创建一个KvmBox对象
 */
KvmBox* new_kvmBox();


/**
 * 删除创建的KvmBox对象
 */
bool_t del_kvmBox(KvmBox *box);


/**
 * 创建一个KvmBoxs对象
 */
KvmBoxs* new_kvmBoxs();


/**
 * 删除一个KvmBoxs对象
 */
bool_t del_kvmBoxs(KvmBoxs *boxs);


#endif //__KVMBOX_H