/**
 * MMU页表
 * API实现
 * 
 * 说明:
 * · 暂时只是实现了2M分页MMU
 */
#include"global.h"
#include"mmu_t.h"

void mmu_init(Machine *mach){
	//页表指针
	u64_t *p1 = (u64_t*)(MMU_ADR+0x1000*0);	//一级页目录(4KB,512)
	u64_t *p2 = (u64_t*)(MMU_ADR+0x1000*1);	//二级页目录(4KB,512)
	u64_t *p3 = (u64_t*)(MMU_ADR+0x1000*2);	//三级页目录(4KB,512)
	u64_t addr = 0;	//从物理地址0开始映射
	
	//初始化
	memset(MMU_ADR,0,MMU_SIZE);	//MMU页表总大小

	/*
	 * MMU平坦映射
	 * 页目录项是4K对齐的，因此每个页目录项地址低12位为0，所以可以进行或运算
	 * ·页表项由子页表地址(4K对齐)和标志位合成
	 */
	for(uint_t i1 = 0; i1 < MMU_PT1_SIZE; i1++){	//一级页目录
		//页地址;页面存在位;页面读写位
		p1[i1] = (u64_t)((u32_t)p2 | MMU_PT1_RW | MMU_PT1_P);
		
		for(uint_t i2 = 0; i2 < MMU_PT2_SIZE; i2++){	//二级页目录
			//页地址;页存在位;页读写位
			p2[i2] = (u64_t)((u32_t)p3 | MMU_PT2_RW | MMU_PT2_P);
			
			for(uint_t i3 = 0; i3 < MMU_PT3_SIZE; i3++){	  //三级页目录
				//页地址;页存在位;页读写位;2MB分页位
				p3[i3] = 0 | addr | MMU_PT3_PS | MMU_PT3_RW | MMU_PT3_P;	
				//页地址递增2MB
				addr += MMU_P_SIZE;
			}
			//指向下一个三级页表
			p3 = (u64_t*)((u32_t)p3 + MMU_PT_SIZE);
		}
		//指向下一个二级页表
		p2 = (u64_t*)((u32_t)p2+MMU_PT_SIZE);
	}

	//实际映射（求出KERNEL_VIRADR_START的一级页目录索引，使得对应目录项也指向唯一的二级页表）
	p1[(KERNEL_VIRADR_START>>MMU_PT1_SHIFT) & 0x1ff] = p1[0];

	//填写机器信息
	mach->mmu_addr = (u64_t)MMU_ADR;			//页表地址
	mach->mmu_size = (u64_t)MMU_SIZE;			//页表总大小
	mach->viraddr_size = (u64_t)0x400000000ULL;	//内核虚拟地址空间大小
	
	return;
}

void mmu_display(Machine mach){
	printk("=================================mmu info====================================\n");
	printk("mmu page address:0x%lx\n",mach.mmu_addr);
	printk("mmu page size:0x%lx\n",mach.mmu_size);
	printk("virtual memory size:0x%lx\n",mach.viraddr_size);
	printk("=================================mmu end.====================================\n");
	return;
}