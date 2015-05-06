/*
 * Knuth's man or boy test
 *
   gcc -Wall -O3 -std=gnu11 -omanorboy manorboy.c
 *
 * Author: Martin Uecker <uecker@eecs.berkeley.edu>
 */

#include <stdio.h>

int pone(void) { return +1; }
int mone(void) { return -1; } 
int zero(void) { return 0; }

typedef int (*fun)(void);

int manorboy(int k, fun x1, fun x2, fun x3, fun x4, fun x5)
{
	int B(void)
	{
		return manorboy(--k, B, x1, x2, x3, x4);
	}

	return (k <= 0) ? (x4() + x5()) : (B());
}

int main(void)
{
	printf("%d\n", manorboy(10, pone, mone, mone, pone, zero));
	return 0;
}

