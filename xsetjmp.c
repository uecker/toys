
#include "xsetjmp.h"

extern inline _Noreturn void xlongjmp(xjmp_buf env, int r);



#ifdef XTEST
#include <stdio.h>

int main()
{
	xjmp_buf env;
	int r = 1;

	if (0 == (r = xsetjmp(env)))
		xlongjmp(env, 3);

	printf("%d\n", r);
}
#endif

