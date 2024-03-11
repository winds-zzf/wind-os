/**
 * 树结构
 * 类型定义
 */
#ifndef __TREE_T_H
#define __TREE_T_H

/**
 * 二叉树结构体
 */
typedef struct Tree{
	struct TNODE *lchild,*rchild;		//左右孩子节点
}Tree;

/**
 * 可不可以将Hash值作为建树的依据，每一个Hash值都唯一对应节点的路径
 */

#endif //__TREE_T_H