#include "lu_erron.h"
#include "stdio.h"

void IsPrime(int n) 
{
    if (n < 2) {
        printf("Number is not prime\n");
    }
    for (int i = 2; i <= n / 2; ++i) {
        if (n % i == 0) {
            printf("Number is not prime\n");
            return;
        }
        
    }
    printf("Number is prime\n");
    
}

