/*
 * stack machine
 *
   gcc -Wall -O3 -std=gnu11 -ostackvm stackvm.c
 *
 * Author: Martin Uecker <uecker@eecs.berkeley.edu>
 */

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

enum byte_code {

	ADD, SUB, MUL, DIV, 
	AND, OR , XOR, NOT, 
	JPC, JMP, CAL, RET,
	LIT, INT, LOD, 
};


int vm_run(int* code, int* stack, int i, int t, int a)
{
	int pop(void) { return stack[--t]; }
	void push(int x) { stack[t++] = x; }
	int load(void) { return code[i++]; }

	while (true) {

		switch (load()) {

		case ADD: { a += pop(); } break;
		case SUB: { a -= pop(); } break;
		case MUL: { a *= pop(); } break;
		case DIV: { a /= pop(); } break;
		case AND: { a &= pop(); } break;
		case OR : { a |= pop(); } break;
		case XOR: { a ^= pop(); } break;
		case NOT: { a = ~a; } break;
		case JPC: { int l = load(); if (a) i = l; } break;
		case JMP: { i = load(); } break;
		case LIT: { push(a); a = load(); } break;
		case INT: { t -= load(); } break;
		case LOD: { push(a); a = stack[t - 1 - load()]; } break;
		case CAL: { a = vm_run(code, stack, load(), t, a); } break;
		case RET: { return a; } break;

		default: assert(0);
		}

		assert(t >= 0);
	} 
}


int main(void)
{
	int stack[100];
	int factorial[] = {

		LIT, 1,
		LOD, 1,
		SUB,
		JPC, 10,
		LIT, 1,
		RET,
		CAL, 0,	// 10:
		LOD, 1,
		MUL,
		RET
	};

	printf("%d\n", vm_run(factorial, stack, 0, 0, 7));
	return 0;
}





