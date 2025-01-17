#include <stdio.h>
#include <stdlib.h>


int main(int argc, char ** argv){

char * p1 = (char *)malloc(1);
char * p2 = (char *) malloc(26);
char * p3 = (char *) malloc(1);

printf("p1 : %p \n",p1);
printf("p2 : %p \n",p2);
printf("p3 : %p \n",p3);
return 0;

}
