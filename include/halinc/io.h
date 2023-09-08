/**
 * 通用IO控制
 * API定义
 */

/**
 *
 */
INLINE void out_u8(const u16_t port, const u8_t val){
    __asm__ __volatile__("outb  %1, %0\n"
                         :
                         : "dN"(port), "a"(val));
}


/**
 *
 */
INLINE void out_u8_p(const u16_t port, const u8_t val){
    __asm__ __volatile__("outb  %1, %0\n\t"
                         "nop \n\t"
                         "nop \n\t"
                         :
                         : "dN"(port), "a"(val));
}


/**
 *
 */
INLINE u8_t in_u8(const u16_t port){
    u8_t tmp;
    /* GCC can optimize here if constant */
    __asm__ __volatile__("inb %1, %0\n"
                         : "=a"(tmp)
                         : "dN"(port));
    return tmp;
}


/**
 *
 */
INLINE void out_u16(const u16_t port, const u16_t val){
    __asm__ __volatile__("outw  %1, %0\n"
                         :
                         : "dN"(port), "a"(val));
}


/**
 *
 */
INLINE u16_t in_u16(const u16_t port){
    u16_t tmp;
    __asm__ __volatile__("inw %1, %0\n"
                         : "=a"(tmp)
                         : "dN"(port));
    return tmp;
}


/**
 *
 */
INLINE void out_u32(const u16_t port, const u32_t val){
    __asm__ __volatile__("outl  %1, %0\n"
                         :
                         : "dN"(port), "a"(val));
}


/**
 *
 */
INLINE u32_t in_u32(const u16_t port){
    u32_t tmp;
    __asm__ __volatile__("inl %1, %0\n"
                         : "=a"(tmp)
                         : "dN"(port));
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
        : "memory");
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
        : "memory");
    return;
}
