#ifndef __ASM_ATOMIC_H
#define __ASM_ATOMIC_H

typedef struct {
	volatile int counter;
} atomic_t;

#define ATOMIC_OP_RETURN(op, c_op, asm_op)                                  \
static inline int  atomic_##op (int  i, atomic_t  *v)                 \
{                                                                           \
    ulong   ultemp;                                                         \
    int     result;                                                        \
                                                                            \
    __asm__ __volatile__(                                                   \
        "1: ldxr            %w0, %2         \n"                             \
        "   " #asm_op "     %w0, %w0, %w3   \n"                             \
        "   stlxr           %w1, %w0, %2    \n"                             \
        "   cbnz            %w1, 1b         \n"                             \
        "   dmb ish"                                                        \
            : "=&r" (result), "=&r" (ultemp), "+Q" (v->counter)            \
            : "Ir" (i)                                                      \
            : "memory");                                                    \
                                                                            \
    return  (result);                                                      \
}
 
ATOMIC_OP_RETURN(add,  +=,  add)
ATOMIC_OP_RETURN(sub,  -=,  sub)
ATOMIC_OP_RETURN(and,  &=,  and)
ATOMIC_OP_RETURN(or,   |=,  orr)
ATOMIC_OP_RETURN(xor,  ^=,  eor)

#endif
