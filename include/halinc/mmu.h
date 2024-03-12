/**
 * MMU管理模块
 * API定义
 */
#ifndef __MMU_H
#define __MMU_H

#include "_string.h"
#include "mmu_t.h"


/**
 * 
 */
bool_t mmu_transform(mmu_t *mmu,addr_t vadr,addr_t padr,u64_t flags);
/**
 * 
 */
addr_t mmu_untransform(mmu_t *mmu,addr_t vadr);
/**
 * 
 */
void mmu_load(mmu_t *mmu);
/**
 * 
 */
void mmu_refresh();
/**
 * 
 */
bool_t mmu_init(mmu_t *mmu);

/**
 * 
 */
bool_t mmu_clean(mmu_t *mmu);



/**
 * 初始化cr3寄存器
 */
INLINE void cr3_init(cr3_t *cr3){
	if(NULL==cr3){
		return ;
	}
	//使用共用体的entry名直接将cr3初始化为0
	cr3->entry = 0;
	return;
}


/**
 * 初始化MMU
 */
INLINE void mmu_t_init(mmu_t *mmu){
	if(NULL==mmu){
		return;
	}
	spinlock_init(&mmu->lock);
	mmu->status = 0;
	mmu->flag = 0;

	cr3_init(&mmu->cr3);
	mmu->pt1 = NULL;

	//初始化链表
	list_t_init(&mmu->pt1s);
	mmu->pt1sNum = 0;
	list_t_init(&mmu->pt2s);
	mmu->pt2sNum = 0;
	list_t_init(&mmu->pt3s);
	mmu->pt3sNum = 0;
	list_t_init(&mmu->pt4s);
	mmu->pt4sNum = 0;

	return;
}


/**
 * 初始化各级页表arrays
 */
INLINE void pt1_init(pt1_t *pt1){
	if(NULL==pt1){
		return;
	}
	memset(pt1,0,sizeof(pt1_t));
	return;
}
INLINE void pt2_init(pt2_t *pt2){
	if(NULL==pt2){
		return;
	}
	memset(pt2,0,sizeof(pt2_t));
	return;
}
INLINE void pt3_init(pt3_t *pt3){
	if(NULL==pt3){
		return;
	}
	memset(pt3,0,sizeof(pt3_t));
	return;
}
INLINE void pt4_init(pt4_t *pt4){
	if(NULL==pt4){
		return;
	}
	memset(pt4,0,sizeof(pt4_t));
	return;
}



/**
 * 从物理地址中获取各级页表索引idx
 */
INLINE uint_t pt1_idx(addr_t adr){
	return (uint_t)((adr>>MMU_IDX1_SHIFT)&MMU_IDX1_MASK);
}
INLINE uint_t pt2_idx(addr_t adr){
	return (uint_t)((adr>>MMU_IDX2_SHIFT)&MMU_IDX2_MASK);
}
INLINE uint_t pt3_idx(addr_t adr){
	return (uint_t)((adr>>MMU_IDX3_SHIFT)&MMU_IDX3_MASK);
}
INLINE uint_t pt4_idx(addr_t adr){	
	return (uint_t)((adr>>MMU_IDX4_SHIFT)&MMU_IDX4_MASK);
}



/**
 * 判断二、三、四级页表是否是空表(一级页表非空不检查)
 */
/*
 * 一级页表一定不为空，所以不进行检查
INLINE bool_t pt1_is_empty(pt1_t *pt1){
	for(uint_t i=0;i<MMU_PT1_MAX;i++){
		if(0 != pt1->entries[i].entry){
			return FALSE;
		}
	}
	return TRUE;
}
*/
INLINE bool_t pt2_is_empty(pt2_t *pt2){
	for(uint_t i=0;i<MMU_PT2_MAX;i++){
		if(0 != pt2->entries[i].entry){
			return FALSE;
		}
	}
	return TRUE;
}
INLINE bool_t pt3_is_empty(pt3_t *pt3){
	for(uint_t i=0;i<MMU_PT3_MAX;i++){
		if(0 != pt3->entries[i].entry){
			return FALSE;
		}
	}
	return TRUE;
}
INLINE bool_t pt4_is_empty(pt4_t *pt4){
	for(uint_t i=0;i<MMU_PT4_MAX;i++){
		if(0 != pt4->entries[i].entry){
			return FALSE;
		}
	}
	return TRUE;
}


/**
 * 判断页表项地址是否为空
 * 可以优化实现以提高处理速度
 */
INLINE bool_t pt1entry_is_have(pt1entry_t *entry){
	if(entry->flags.adr>0){
		return TRUE;
	}
	return FALSE;
}
INLINE bool_t pt2entry_is_have(pt2entry_t *entry){
	if(entry->flags.adr>0){
		return TRUE;
	}
	return FALSE;
}
INLINE bool_t pt3entry_is_have(pt3entry_t *entry){
	if(entry->flags.adr>0){
		return TRUE;
	}
	return FALSE;
}
INLINE bool_t pt4entry_is_have(pt4entry_t *entry){
	if(entry->flags.adr>0){
		return TRUE;
	}
	return FALSE;
}


/**
 * 检查也是否存在于内存，只能用于判断这个页表项对应的页是否在内存，并不能判断这个页面是否存在，
 * 所以说只有这个表项对应的页存在，p位才有效
 * 可以优化实现以提高处理速度
 */
INLINE bool_t pt1entry_is_present(pt1entry_t *entry){
	if(1==entry->flags.p){
		return TRUE;
	}
	return FALSE;
}
INLINE bool_t pt2entry_is_present(pt2entry_t *entry){
	if(1==entry->flags.p){
		return TRUE;
	}
	return FALSE;
}
INLINE bool_t pt3entry_is_present(pt3entry_t *entry){
	if(1==entry->flags.p){
		return TRUE;
	}
	return FALSE;
}
INLINE bool_t pt4entry_is_present(pt4entry_t *entry){
	if(1==entry->flags.p)
	return FALSE;
}


/**
 * 获取各级页目录项中的页物理地址
 */
INLINE addr_t pt1entry_padr(pt1entry_t *entry){
	return entry->flags.adr<<12;
}
INLINE addr_t pt2entry_padr(pt2entry_t *entry){
	return entry->flags.adr<<12;
}
INLINE addr_t pt3entry_padr(pt3entry_t *entry){
	return entry->flags.adr<<12;
}
INLINE addr_t pt4entry_padr(pt4entry_t *entry){
	return entry->flags.adr<<12;
}


/**
 * 获取各级页目录项中的虚拟地址
 */
INLINE addr_t pt1entry_vadr(pt1entry_t *entry){
	return MAP_RV(entry->flags.adr<<12);
}
INLINE addr_t pt2entry_vadr(pt2entry_t *entry){
	return MAP_RV(entry->flags.adr<<12);
}
INLINE addr_t pt3entry_vadr(pt3entry_t *entry){
	return MAP_RV(entry->flags.adr<<12);
}
INLINE addr_t pt4entry_vadr(pt4entry_t *entry){
	return MAP_RV(entry->flags.adr<<12);
}


/**
 * 从页表项中获取下一级页表/页，一级页表是驻留的，因此不需要特别获取
 * 注意返回的是物理地址
 */
INLINE pt2_t* pt1entry_get_pt2(pt1entry_t *entry){
	return (pt2_t*)MAP_RV((addr_t)(entry->flags.adr<<12));
}
INLINE pt3_t* pt2entry_get_pt3(pt2entry_t *entry){
	return (pt3_t*)MAP_RV((addr_t)(entry->flags.adr<<12));
}
INLINE pt4_t* pt3entry_get_pt4(pt3entry_t *entry){
	return (pt4_t*)MAP_RV((addr_t)(entry->flags.adr<<12));
}


#endif //__MMU_H