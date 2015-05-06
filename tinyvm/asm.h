



typedef union ins2_u ins2_t;

// instruction pointer
#define IP 0u
// stack pointer
#define SP 1u
// used by assembler
#define AR 2u
// constant 0
#define C0 3u
// constant 1
#define C1 4u
// frame pointer
#define FP 5u
// return register
#define RR 6u

#define U1 8u
#define U2 9u
#define U3 10u
#define U4 11u

#define A1 12u
#define A2 13u
#define A3 14u
#define A4 15u


extern void cnst(ins2_t** p, unsigned char a, unsigned int b);
extern void move(ins2_t** p, unsigned char a, unsigned char b);

extern void add(ins2_t** p, unsigned char a, unsigned char b, unsigned char c);
extern void sub(ins2_t** p, unsigned char a, unsigned char b, unsigned char c);
extern void mul(ins2_t** p, unsigned char a, unsigned char b, unsigned char c);
extern void adiv(ins2_t** p, unsigned char a, unsigned char b, unsigned char c);

extern void and(ins2_t** p, unsigned char a, unsigned char b, unsigned char c);
extern void or(ins2_t** p, unsigned char a, unsigned char b, unsigned char c);
extern void xor(ins2_t** p, unsigned char a, unsigned char b, unsigned char c);

extern void push(ins2_t** p, unsigned char a);
extern void pop(ins2_t** p, unsigned char a);

extern void ret(ins2_t** p);
extern void enter(ins2_t** p, int s);
extern void leave(ins2_t** p, unsigned char a);
//extern void call(ins2_t** p, int s, unsigned char a);
extern void stop(ins2_t** p);
extern void cjmp(ins2_t** p, unsigned char a, int b);
extern void jump(ins2_t** p, int b);

extern void call(ins2_t** p, int s, unsigned char a);
extern void call2(ins2_t** p, int s, unsigned char a);

extern void load(ins2_t** p, unsigned char a, int v);
extern void store(ins2_t** p, int v, unsigned char a);
extern void arg(ins2_t** p, unsigned char a, int v);

extern ins2_t* here(ins2_t**p);
extern void from(ins2_t**p, ins2_t* j);

extern void fp(ins2_t** p, int i);


extern void insn_print(ins2_t x);
extern int tramp(ins2_t** p, char r, int v);




