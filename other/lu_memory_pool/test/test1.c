#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


#define BLOCK_SIZE  16   //每次分配内存块大小
#define TEST_COUNT  1024

#define MEM_POOL_SIZE (1024 * 4) //内存池每块大小



int main(int argc, char **argv)
{
	int i = 0, k = 0;
	int use_free = 0;


	{
	    char * ptr[TEST_COUNT];
	    for(k = 0; k< 1024 * 500; k++){
		for(i = 0; i < TEST_COUNT ; i++)
                {
                    ptr[i] = malloc(BLOCK_SIZE);
		    if(!ptr[i]) fprintf(stderr,"malloc failed. reason:%s\n",strerror(errno));
		    else{
		         *ptr[i] = '\0';
			 *(ptr[i] +  BLOCK_SIZE - 1) = '\0';
		    }
                }

		for(i = 0; i < TEST_COUNT ; i++){
		    if(ptr[i]) free(ptr[i]);
		}
	    }

	}
	return 0;
}
