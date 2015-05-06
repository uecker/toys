      
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/mman.h>

#ifndef _POSIX_MAPPED_FILES
#error POSIX_MAPPED_FILES
#endif

#include "baum.h"

int main(int argc, char* argv[argc])
{
	int i;
	int fd;
	const char* string;
	struct find_result_s frs;
	struct suffixtree_s sts;

	if (argc != 3)
		abort();

	if ((fd = open(argv[1], O_RDONLY)) < 0) {

		perror("mmap test");
		abort();
	}

	struct stat st;
        
        if (-1 == fstat(fd, &st))
                abort();

	if (MAP_FAILED == (string = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0))) {

		perror("mmap test 2");
		abort();
	}
			
	if (close(fd) < 0) {

		perror("mmap test 3");
		abort();
	}
	
//	printf("String: %s\n", string);
	
	sts = create_suffixtree(string);
	find(sts.root, argv[2], &frs);

	for (i = frs.from; i <= frs.to; i++)
		printf("Treffer %d, %.20s\n", sts.table[i], 
				sts.text + sts.table[i]);
				
	
        if (munmap((void*)string, st.st_size) < 0) {

		perror("mmap test 4");
		abort();
	}
}



