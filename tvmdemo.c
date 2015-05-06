/* 
 * factorial and man-or-boy-test for tiny vm
 *
   gcc -std=gnu11 -Wall -O2 -otvmdemo tvmdemo.c tinyvm/asm.c tinyvm/cpu.c
 *
 * Author: Martin Uecker <uecker@eecs.berkeley.edu>
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "tinyvm/cpu.h"
#include "tinyvm/asm.h"



void prelude(ins2_t** p, unsigned int start)
{
	cnst(p, C0, 0);
	cnst(p, C1, 1);
	cnst(p, SP, 32);	// stack starts at 32
	cnst(p, FP, 32);
	push(p, RR);
	cnst(p, 10, start);	// jump to start
	call(p, 1, 10);
	stop(p);
}

void factorial(ins2_t** p, unsigned int start)
{
ins2_t* base = here(p);

ins2_t* f = here(p);

	enter(p, 0);
	arg(p, U1, 0);
	sub(p, U1, U1, C1);

ins2_t* j = here(p);

	cjmp(p, U1, -1);
	leave(p, C1);

from(p, j);

	push(p, U1);
	cnst(p, U2, start + (f - base));
	call(p, 1, U2); 
	arg(p, U2, 0);
	mul(p, U2, U2, RR);
	leave(p, U2);
}



void manorboy(ins2_t** p, unsigned int start)
{
ins2_t* base = here(p);

ins2_t* j = here(p);
	jump(p, -1);

ins2_t* po = here(p);
	move(p, RR, C1);
	ret(p);

ins2_t* mo = here(p);
	sub(p, RR, C0, C1);
	ret(p);

ins2_t* zo = here(p);
	move(p, RR, C0);
	ret(p);

ins2_t* b = here(p);
	enter(p, 0);

//	move(p, U2, FP); // save frame pointer 
// -- we don't need to because we restore SP directly

	fp(p, 1); 		// get stack frame
	load(p, A1, 0);
	sub(p, A1, A1, C1);
	store(p, 0, A1);

int t = tramp(p, A2, start + (b - base));

	// prepare call to manorboy

	load(p, A3, 1);
	load(p, A4, 2);
	load(p, U1, 3);
	push(p, U1);
	arg(p, U1, 1);
	push(p, U1);

//	move(p, FP, U2); // restore frame pointer

ins2_t* l = here(p);
	cnst(p, U1, -1);
	call(p, 2, U1);

	//leave(p, RR);

	cnst(p, AR, t);
	sub(p, SP, SP, AR);
	pop(p, FP);
	ret(p);


ins2_t* mob = here(p);
	cnst(&l, U1, start + (mob - base));

	// store first arguments as local variables

	enter(p, 4);
	store(p, 0, A1);
	store(p, 1, A2);
	store(p, 2, A3);
	store(p, 3, A4);


	sub(p, U1, C0, A1);
	cnst(p, U2, (1u << 31));
	and(p, U1, U1, U2);

ins2_t* d = here(p);
	cjmp(p, U1, -1);

	// k <= 0
	arg(p, U1, 1);
	call(p, 0, U1);
	push(p, RR);
	arg(p, U1, 0);
	call(p, 0, U1);
	pop(p, U2);
	add(p, RR, U2, RR);
	leave(p, RR);

from(p, d); // k > 0
	cnst(p, U1, start + (b - base));
	call2(p, 0, U1);
	leave(p, RR);

from(p, j);

	cnst(p, A1, 10);
	cnst(p, A2, start + (po - base));
	cnst(p, A3, start + (mo - base));
	cnst(p, A4, start + (mo - base));
	cnst(p, U1, start + (po - base));
	push(p, U1);
	cnst(p, U1, start + (zo - base));
	push(p, U1);
	cnst(p, U2, start + (mob - base)); // manorboy
	call(p, 2, U2);
	ret(p);
}



int main(int argc, char** argv)
{
	ins2_t* mm = malloc(100000 * sizeof(ins2_t));
	ins2_t* p = &mm[0];


	unsigned int start = 50000;
	prelude(&p, start);

	p = &mm[start];

	factorial(&p, start);
	//manorboy(&p, start);

	printf("%ld words.\n", p - &mm[start]);
	
	
	reg_t reg[256] = { [0 ... 255] = 0 };
	reg[0] = 0;

	reg[RR] = 7;
	vm(reg, (mem_t*)mm);

	assert(32 == reg[SP]);

	printf("Result: %d\n", reg[RR]);
	return 0;
}

