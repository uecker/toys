/* Author: 2020 Martin Uecker
 *
 * An implementation of setjmp/longjmp using GCC's nested functions.
 * This function does not require the use of the volatile qualifier.
 *
 * (Thanks to Jens Gustedt for comments).
 */

#ifndef __XSETJMP_H
#define __XSETJMP_H 1


typedef struct {
	 __attribute__((__noreturn__)) void (*fun)(int);
} xjmp_buf[1];


/*
 * A local variable for the return value in xsetjmp should also work,
 * although the store in the local function would then occur out of its
 * lifetime (before the block is entered again with the goto). But one
 * could argue that calling the nested functions also already enters
 * the block.
 **/

#define xsetjmp(env) 					\
({							\
  	__label__ trgt;					\
	static _Thread_local int __xsetjmp_ret = 0; 	\
	__attribute__((__noreturn__)) void __jmp(int r) \
	{						\
 		__xsetjmp_ret = r ?: 1;			\
 		goto trgt;				\
	}						\
  	env[0].fun = __jmp;				\
trgt:;	int tmp = __xsetjmp_ret;   			\
	__xsetjmp_ret = 0;				\
	tmp;						\
})

inline _Noreturn void xlongjmp(xjmp_buf env, int r) { env[0].fun(r); }

#endif // __XSETJMP_H

