
#ifndef __CPU_H
#define __CPU_H 1

#include <stdint.h>

typedef int32_t ins_t;
typedef int32_t reg_t;
typedef int32_t mem_t;


enum byte_code { 

        STP,
	ADD, SUB, MUL, DIV, MOD,
        AND, OR, XOR, LIT,
        STO, LOD, MOV
};

typedef union ins2_u {
	unsigned char b[4];
	ins_t w;
} ins2_t;

extern void vm(reg_t regs[256], mem_t* mem);
extern int vm_step(reg_t regs[256], mem_t* mem);


#endif
