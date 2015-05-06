/*
 * "assembler" for tiny cpu
 *
 * Author: Martin Uecker <uecker@eecs.berkeley.edu>
 *
 * ABI:
 * 
 * reg[0] instruction pointer
 * reg[1] stack pointer
 * reg[2] used by assembler in composite instructions
 * reg[3] constant 0
 * reg[4] constant 1
 * reg[5] frame pointer
 * reg[6] return value
 * reg[7] reserved
 *
 * reg[8-11] caller saved
 * reg[12-15] arguments
 *
 * stack frame:
 *
 *	  arguments
 *	  SLP static link pointer
 *        IP1 return adress
 * FP  -> FP1 old frame pointer
 *        local variables  
 */

#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "asm.h"


static void insn(ins2_t** p, unsigned char u, unsigned char v, unsigned char w, unsigned char x)
{
	(*p)->b[0] = u;
	(*p)->b[1] = v;
	(*p)->b[2] = w;
	(*p)->b[3] = x;
	(*p)++;
}


const char* instr_names[] = { "stp", "add", "sub", "mul", "div", "mov", "and", "or", "xor", "lit", "sto", "lod", "mov" };
const char* reg_names[] = { "ip", "sp", "ar", "c0", "c1", "fp", "rr", "--", "u0", "u1", "u2", "u3", "u4", "u5", "u6", "u7" };


void insn_print(ins2_t x)
{
	switch (x.b[0]) {
	case STP:
		printf("stp\n");
		break;
	case STO ... MOV:
	case ADD ... XOR:
		printf("%s %s %s %s\n", instr_names[x.b[0]], reg_names[x.b[1]], reg_names[x.b[2]], reg_names[x.b[3]]);
		break;
	case LIT:
		printf("%s %s %d\n", instr_names[x.b[0]], reg_names[x.b[1]], (reg_t)(x.b[3]) * 256 + (reg_t)(x.b[2]));
		break;
	default:
		printf("NOA %d %d %d %d\n", x.b[0], x.b[1], x.b[2], x.b[3]);
		break;
	}
}



void move(ins2_t** p, unsigned char a, unsigned char b) { insn(p, MOV, C1, a, b); }
void peek(ins2_t** p, unsigned char a, unsigned char b) { insn(p, LOD, a, b, C0); }
void poke(ins2_t** p, unsigned char a, unsigned char b) { insn(p, STO, a, b, C0); }

void cnst(ins2_t** p, unsigned char a, unsigned int b) 
{
	if (0 == b) {

		insn(p, XOR, a, a, a);

	} else {

		insn(p, LIT, a, (b >> 16) % 256, (b >> 24) % 256);
		insn(p, LIT, a, (b >> 0) % 256, (b >> 8) % 256);
	}
}

void add(ins2_t** p, unsigned char a, unsigned char b, unsigned char c) { insn(p, ADD, a, b, c); }
void sub(ins2_t** p, unsigned char a, unsigned char b, unsigned char c) { insn(p, SUB, a, b, c); }
void mul(ins2_t** p, unsigned char a, unsigned char b, unsigned char c) { insn(p, MUL, a, b, c); }
void adiv(ins2_t** p, unsigned char a, unsigned char b, unsigned char c) { insn(p, DIV, a, b, c); }
void mod(ins2_t** p, unsigned char a, unsigned char b, unsigned char c) { insn(p, MOD, a, b, c); }

void and(ins2_t** p, unsigned char a, unsigned char b, unsigned char c) { insn(p, AND, a, b, c); }
void or(ins2_t** p, unsigned char a, unsigned char b, unsigned char c) { insn(p, OR, a, b, c); }
void xor(ins2_t** p, unsigned char a, unsigned char b, unsigned char c) { insn(p, XOR, a, b, c); }

void push(ins2_t** p, unsigned char a) { add(p, SP, SP, C1); insn(p, STO, SP, a, C0); }
void pop(ins2_t** p, unsigned char a) { sub(p, SP, SP, C1); insn(p, LOD, a, SP, C1); }

void ret(ins2_t** p) { pop(p, AR); add(p, IP, AR, C1); };
void retn(ins2_t** p, int s) { pop(p, AR); add(p, IP, AR, C1); if (s) { cnst(p, AR, s); sub(p, SP, SP, AR); } };

// save old frame pointer, save stack pointer in frame pointer, make room for local variables
void enter(ins2_t** p, int s) { push(p, FP); move(p, FP, SP); cnst(p, AR, s); add(p, SP, SP, AR); }
// restore stack pointer, restore old frame pointer, move result into return register, return
void leave(ins2_t** p, unsigned char a) { move(p, SP, FP); pop(p, FP); if (a != RR) move(p, RR, a); ret(p); }
//void leave(ins2_t** p, unsigned char a) { move(p, SP, FP); pop(p, FP); move(p, RR, a); ret(p); }

void stop(ins2_t** p) { insn(p, STP, 0, 0, 0); }

void cjmp(ins2_t** p, unsigned char a, int b) { cnst(p, AR, b - 3); add(p, AR, IP, AR); insn(p, MOV, a, IP, AR); }
void jump(ins2_t** p, int b) { cjmp(p, C1, b); }

void call(ins2_t** p, int s, unsigned char a) { push(p, C0); push(p, IP); move(p, IP, a); if (s) { cnst(p, AR, s + 1); sub(p, SP, SP, AR); } else sub(p, SP, SP, C1); }
//void call(ins2_t** p, int s, unsigned char a) { push(p, IP); move(p, IP, a); if (s) { cnst(p, AR, s); sub(p, SP, SP, AR); }  }
void call2(ins2_t** p, int s, unsigned char a) { push(p, FP); push(p, IP); move(p, IP, a); if (s) { cnst(p, AR, s + 1); sub(p, SP, SP, AR); } else sub(p, SP, SP, C1); }

void load(ins2_t** p, unsigned char a, int v) { cnst(p, AR, v + 1); insn(p, LOD, a, FP, AR); }
void store(ins2_t** p, int v, unsigned char a) { cnst(p, AR, v + 1); insn(p, STO, FP, a, AR); }
//void arg(ins2_t** p, unsigned char a, int v) { cnst(p, AR, v + 2); sub(p, AR, C0, AR); insn(p, LOD, a, FP, AR); }
void arg(ins2_t** p, unsigned char a, int v) { cnst(p, AR, v + 3); sub(p, AR, C0, AR); insn(p, LOD, a, FP, AR); }

ins2_t* here(ins2_t**p) { return *p; }
void from(ins2_t**p, ins2_t* j) { ins2_t* l = j; cnst(&l, AR, (here(p) - j) - 3); }
// we overwrite the cnst here: FIXME: cnst may change in size


void fp(ins2_t** p, int i)
{
	while (i--) 
		arg(p, FP, -1);
}


int tramp(ins2_t** p, char r, int v) 
{ 
	// return address of trampoline in reg r

	move(p, r, SP);
	add(p, r, r, C1);

	// create code on stack which loads the constant
	// corresponding to the current frame pointer into RR

//	cnst(&tp, RR, [FP]);

	cnst(p, AR, 256 * 256);
	mul(p, AR, AR, FP);
	insn(p, LIT, AR, LIT, RR);
	push(p, AR);

	cnst(p, AR, 256 * 256);
	mod(p, AR, FP, AR);
	insn(p, LIT, AR, LIT, RR);
	push(p, AR);

	// create code to load the static link pointer
	// and jump to address given in v

	ins2_t t[10];
	ins2_t* tp = &t[0];

	sub(&tp, AR, C0, C1);
	insn(&tp, STO, SP, RR, AR);	// we use RR here
	cnst(&tp, AR, v);
	move(&tp, IP, AR);

	int ts = (int)(tp - &t[0]);

	for (int i = 0; i < ts; i++) {

		cnst(p, AR, t[i].w);
		push(p, AR);
	}		

	// return the size of the trampoline

	return 2 + ts;
}



