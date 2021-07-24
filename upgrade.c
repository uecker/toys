/* Copyright 2021. Martin Uecker
 *
 * gcc -Wall -O2 -z noexecstack upgrade.c
 *
 *
 * */

#include <assert.h>
#include <stdio.h>

#if __GNUC__ >= 5
#define NESTED_CHAIN(p) 	\
({														\
	struct { unsigned short mov1; unsigned int addr; unsigned short mov2; void* chain; unsigned int jmp; } 	\
		__attribute__((packed))* __t = (void*)p;							\
	assert((0xbb41 == __t->mov1) && (0xba49 == __t->mov2) && (0x90e3ff49 == __t->jmp));			\
	__t->chain;												\
})
#define NESTED_ADDR(p) 	\
({														\
	__auto_type __p = (p);											\
	struct { unsigned short mov1; unsigned int addr; unsigned short mov2; void* chain; unsigned int jmp; } 	\
		__attribute__((packed))* __t = (void*)__p;							\
	assert((0xbb41 == __t->mov1) && (0xba49 == __t->mov2) && (0x90e3ff49 == __t->jmp));			\
	(typeof(__p))(unsigned long)__t->addr;									\
})
#else
#define NESTED_CHAIN(p)	\
({														\
	struct { unsigned short mov1; void* addr; unsigned short mov2; void* chain; unsigned int jmp; } 	\
		__attribute__((packed))* __t = (void*)p;							\
	assert((0xbb49 == __t->mov1) && (0xba49 == __t->mov2) && (0x90e3ff49 == __t->jmp));			\
	__t->chain;												\
})
#define NESTED_ADDR(p) \
({														\
	__auto_type __p = (p);											\
	struct { unsigned short mov1; void* addr; unsigned short mov2; void* chain; unsigned int jmp; } 	\
			__attribute__((packed))* __t = (void*)__p;						\
	assert((0xbb49 == __t->mov1) && (0xba49 == __t->mov2) && (0x90e3ff49 == __t->jmp));			\
	(typeof(__p))__t->addr;											\
})
#endif


#ifndef __x86_64__
#error only supported on x86_64
#endif


#define NESTED_UPGRADE(self, ptr, args)	\
	if (self != ptr) return __builtin_call_with_static_chain(NESTED_ADDR((typeof(self)*)ptr) args, NESTED_CHAIN(ptr))





int bar(int (*p)(void* data, int x), void* data)
{
	return p(data, 4);
}


int main()
{
	int x = 3;
	int r;

	int foo(void* self, int y)
	{
		NESTED_UPGRADE(foo, self, (self, y));
		return x + y;
	}

	r = bar(NESTED_ADDR(foo), foo);

	printf("%d\n", r);
}




