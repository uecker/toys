/*
 * tiny virtual CPU
 *
 * Author: Martin Uecker <uecker@eecs.berkeley.edu>
 *
 * 256 register
 * fixed size instructions
 * aligned memory access
 */

#include <stdint.h>
#include <stdbool.h>

#include "cpu.h"


#if 1
// gcc produces faster code 
static 
#define vm_step vm_step2
#endif
int vm_step(reg_t regs[256], mem_t* mem)
{
	ins2_t i;
	i.w = mem[regs[0]++];

#define I i.b[0]
#define A regs[i.b[1]]
#define B regs[i.b[2]]
#define C regs[i.b[3]]
#define L (((reg_t)(i.b[3]) << 8) + ((reg_t)i.b[2]))

	switch(I) {
	case ADD: A = B + C; break;
	case SUB: A = B - C; break;
	case MUL: A = B * C; break;
	case DIV: A = B / C; break;
	case MOD: A = B % C; break;
	case AND: A = B & C; break;
	case OR:  A = B | C; break;
	case XOR: A = B ^ C; break;
	case LIT: A = (A << 16) + L; break;
	case MOV: if (A) B = C; break;
	case LOD: A = mem[B + C]; break;
	case STO: mem[A + C] = B; break;
	case STP:
	default: return 0;
	}

	return 1;
}

void vm(reg_t regs[256], mem_t* mem)
{
	while(vm_step(regs, mem))
		;
}

