/**
 * MMU页表
 * API实现
 * 
 * 说明:
 * · 暂时只是实现了2M分页MMU
 */
#include "globaltype.h"
#include "globalctrl.h"

/**
 * mmu：2MB分页初始化
 * notices:页表中存储的都是物理地址，而不是虚拟地址，因此如果P1，P2，P3是虚拟地址，
 * 在写入上级页表项时应转换为物理地址。
 */
void remmu_init(){
	Machine *mach = &machine;
	//base,p1,p2,p3中的地址必须是物理地址，因为MMU本身就是用于转换虚拟地址的，所以MMU页表中不能包含虚拟地址
	u64_t base = mach->next_addr;		//物理地址
	u64_t *p1 = (u64_t*)(base+0x1000*0);	//一级页目录(4KB,512)
	u64_t *p2 = (u64_t*)(base+0x1000*1);	//二级页目录(4KB,512)
	u64_t *p3 = (u64_t*)(base+0x1000*2);	//三级页目录(4KB,512)
	
	//初始化
	memset(base,0,REMMU_SIZE);	//MMU页表总大小

	/*
	 * MMU平坦映射
	 * 页目录项是4K对齐的，因此每个页目录项地址低12位为0，所以可以进行或运算
	 * ·页表项由子页表地址(4K对齐)和标志位合成
	 */
	u64_t addr = 0;	//从物理地址0开始映射
	for(uint_t i1 = 0; i1 < REMMU_PT1_SIZE; i1++){	//一级页目录
		//页地址;页读写位;页存在位
		p1[i1] = (u64_t)((u64_t)p2 | REMMU_PT1_RW | REMMU_PT1_P);
		
		for(uint_t i2 = 0; i2 < REMMU_PT2_SIZE; i2++){	//二级页目录
			//页地址;页读写位;页存在位
			p2[i2] = (u64_t)((u64_t)p3 | REMMU_PT2_RW | REMMU_PT2_P);
			
			for(uint_t i3 = 0; i3 < REMMU_PT3_SIZE; i3++){	  //三级页目录
				//页地址;2MB分页位;页读写位;页存在位
				p3[i3] = 0 | addr | REMMU_PT3_PS | REMMU_PT3_RW | REMMU_PT3_P;	
				//页地址递增2MB
				addr += REMMU_P_SIZE;
			}
			//指向下一个三级页表
			p3 = (u64_t*)((u64_t)p3 + REMMU_PT_SIZE);
		}
		//指向下一个二级页表
		p2 = (u64_t*)((u64_t)p2+REMMU_PT_SIZE);
	}

	//实际映射（求出KERNEL_VIRADR_START的一级页目录索引，使得对应目录项也指向唯一的二级页表）
	p1[(KRNL_VIRADR_START>>REMMU_PT1_SHIFT) & 0x1ff] = p1[0];

	//填写机器信息
	mach->mmu_addr = (u64_t)base;				//页表地址
	mach->mmu_size = (u64_t)REMMU_SIZE;			//页表总大小
	mach->viraddr_size = (u64_t)0x400000000ULL;	//内核虚拟地址空间大小
	//计算下一个4K对齐的地址
	mach->next_addr = ALIGN_4K(mach->next_addr+mach->mmu_size);

	//设置新页表
	set_cr3(mach->mmu_addr);
	return;
}

void remmu_display(){
	Machine *mach = &machine;
	printk("=================================mmu info====================================\n");
	printk("mmu page address:0x%lx\n",mach->mmu_addr);
	printk("mmu page size:0x%lx\n",mach->mmu_size);
	printk("virtual memory size:0x%lx\n",mach->viraddr_size);
	printk("=================================mmu end.====================================\n");
	return;
}