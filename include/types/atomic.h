/**
 * 原子操作
 * API定义
 */
#ifndef __ATOMIC_H
#define __ATOMIC_H


/**
 * 读取一个原子量的值
 */
INLINE s32_t atomic_read(const atomic_t *v){
	return (*(volatile u32_t *)&(v)->value);
}

/**
 * 将原子量的值设置为i
 */
INLINE void atomic_set(atomic_t *v, int_t i){
	v->value = i;
}

/**
 * 原子加操作
 */
INLINE void atomic_add(int i, atomic_t *v){
	__asm__ __volatile__(
		"lock;"
		"addl %1,%0"
		: "+m"(v->value)
		: "ir"(i)
	);
}

/**
 * 原子减操作
 */
INLINE void atomic_sub(int i, atomic_t *v){
	__asm__ __volatile__(
		"lock;"
		"subl %1,%0"
		: "+m"(v->value)
		: "ir"(i)
	);
}

/**
 * 原子加并测试结果是否为0
 */
INLINE int atomic_add_and_test(int i, atomic_t *v){
	unsigned char c;
	__asm__ __volatile__(
		"lock;"
		"addl %2,%0; sete %1"
		: "+m"(v->value), "=qm"(c)
		: "ir"(i)
		: "memory"
	);
	return c;
}


/**
 * 原子减并测试结果是否为0
 */
INLINE int atomic_sub_and_test(int i, atomic_t *v){
	unsigned char c;
	__asm__ __volatile__(
		"lock;"
		"subl %2,%0; sete %1"
		: "+m"(v->value), "=qm"(c)
		: "ir"(i)
		: "memory"
	);
	return c;
}

/**
 * 原子量自加
 */
INLINE void atomic_inc(atomic_t *v){
	__asm__ __volatile__(
		"lock;"
		"incl %0"
		: "+m"(v->value)
	);
}

/**
 * 原子量自减
 */
INLINE void atomic_dec(atomic_t *v){
	__asm__ __volatile__(
		"lock;"
		"decl %0"
		: "+m"(v->value)
	);
}

/**
 * 原子量自减并测试是否为0
 */
INLINE int atomic_dec_and_test(atomic_t *v){
	unsigned char c;
	__asm__ __volatile__(
		"lock;"
		"decl %0; sete %1"
		: "+m"(v->value), "=qm"(c)
		:
		: "memory"
	);
	return c != 0;
}

/**
 * 原子量自增并测试是否为0
 */
INLINE int atomic_inc_and_test(atomic_t *v){
	unsigned char c;
	__asm__ __volatile__(
		"lock;"
		"incl %0; sete %1"
		: "+m"(v->value), "=qm"(c)
		:
		: "memory"
	);
	return c != 0;
}

/**
 * 引用计数初始化
 */
INLINE void refcount_init(refcount_t* refc){
	atomic_set(refc->ref_count,0);
}

/**
 * 读取引用计数
 */
INLINE void refcount_read(refcount_t* refc){
	atomic_read(refc->ref_count);
}

/**
 * 引用计数自增
 */
INLINE void refcount_inc(refcount_t* refc){
	atomic_inc(refc->ref_count);
}

/**
 * 引用计数自减
 */
INLINE void refcount_dec(refcount_t* refc){
	atomic_dec(refc->ref_count);
	return;
}


#endif //__ATOMIC_H