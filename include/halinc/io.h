/**
 * 通用IO控制
 * API定义
 */

/**
 *
 */
INLINE void out_u8(const u16_t port, const u8_t val){
	__asm__ __volatile__(
		"outb  %1, %0\n"
		:
		: "dN"(port), "a"(val)
	);
}


/**
 *
 */
INLINE void out_u8_p(const u16_t port, const u8_t val){
	__asm__ __volatile__(
		"outb  %1, %0\n\t"
		"nop \n\t"
		"nop \n\t"
		:
		: "dN"(port), "a"(val)
	);
}


/**
 *
 */
INLINE u8_t in_u8(const u16_t port){
	u8_t tmp;
	/* GCC can optimize here if constant */
	__asm__ __volatile__(
		"inb %1, %0\n"
		: "=a"(tmp)
		: "dN"(port)
	);
	return tmp;
}


/**
 *
 */
INLINE void out_u16(const u16_t port, const u16_t val){
	__asm__ __volatile__(
		"outw  %1, %0\n"
		:
		: "dN"(port), "a"(val)
	);
}


/**
 *
 */
INLINE u16_t in_u16(const u16_t port){
	u16_t tmp;
	__asm__ __volatile__(
		"inw %1, %0\n"
		: "=a"(tmp)
		: "dN"(port)
	);
	return tmp;
}


/**
 *
 */
INLINE void out_u32(const u16_t port, const u32_t val){
	__asm__ __volatile__(
		"outl  %1, %0\n"
		:
		: "dN"(port), "a"(val)
	);
	return;
}


/**
 *
 */
INLINE u32_t in_u32(const u16_t port){
	u32_t tmp;
	__asm__ __volatile__(
		"inl %1, %0\n"
		: "=a"(tmp)
		: "dN"(port)
	);
	return tmp;
}

/**
 * 保存当前CPU寄存器的值
 */
INLINE void save_flags_cli(cpuflag_t *flags){
	__asm__ __volatile__(
		"pushfq \t\n"
		"cli    \t\n"
		"popq %0 \t\n"
		: "=m"(*flags)
		:
		: "memory"
	);
	return;
}

/**
 * 恢复当前CPU寄存器的值
 */
INLINE void restore_flags_sti(cpuflag_t *flagsres){
	__asm__ __volatile__(
		"pushq %0 \t\n"
		"popfq \t\n"
		:
		: "m"(*flagsres)
		: "memory"
	);
	return;
}

/**
 * 
 */
INLINE u64_t read_msr(const u32_t reg){
	u32_t eax, edx;
	__asm__ __volatile__(
		"rdmsr"
		: "=a"(eax), "=d"(edx)
		: "c"(reg)
	);
	return (((u64_t)edx) << 32) | (u64_t)eax;
}

/**
 *
 */
INLINE void write_msr(const u32_t reg, const u64_t val){
	__asm__ __volatile__(
		"wrmsr"
		:
		: "a"((u32_t)val), "d"((u32_t)(val >> 32)), "c"(reg)
	);
}


/**
 * 
 */
INLINE u32_t read_kesp(){
	u32_t esp;
	__asm__ __volatile__(
		"movl %%esp,%0"
		: "=g"(esp)
		:
		: "memory"
	);
	return esp;
}

/**
 * 
 */
INLINE u64_t read_rsp(){
	u64_t rsp;
	__asm__ __volatile__(
		"movq %%rsp,%0"
		: "=g"(rsp)
		:
		: "memory"
	);
	return rsp;
}

/**
 * 读取cr2寄存器
 * cr2寄存器中存储缺页地址
 */
INLINE addr_t read_cr2(){
	addr_t regtmp = 0;
	__asm__ __volatile__(
		"movq %%cr2,%0\n\t"
		: "=r"(regtmp)
		:
		: "memory"
	);
	return regtmp;
}


/**
 * 设置cr3寄存器的值
 * pl4adr：4K分页下一级页表地址
 */
INLINE void set_cr3(u64_t pl4adr){
	__asm__ __volatile__(
		"movq %0,%%cr3 \n\t"
		:
		: "r"(pl4adr)
		: "memory" //, "edx"
	);
	return;
}

/**
 * 读取cr3寄存器的值
 */
INLINE u64_t read_cr3(){
	u64_t regtmp = 0;
	__asm__ __volatile__(
		"movq %%cr3,%0\n\t"
		: "=r"(regtmp)
		:
		: "memory"
	);
	return regtmp;
}

/**
 * 写入cr3寄存器的值
 */
INLINE void write_cr3(u64_t r_val){
	__asm__ __volatile__(
		"movq %0,%%cr3 \n\t"
		:
		: "r"(r_val)
		: "memory" //, "edx"
	);
	return;
}







