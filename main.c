#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>


uint32_t squareRoot(uint32_t a_nInput);
bool isPrime(uint32_t num);

bool isPrime(uint32_t num);

int main(int argc, char *argv[]) {
    printf("Sequential :\n");
    uint32_t nbrPrimes = strtoul(argv[1],NULL,10);//Getting number of primes to find from stdin at lunch
    printf("Nbr Primes to find : %d\n",nbrPrimes);

    uint32_t *primes = malloc(nbrPrimes* sizeof(uint32_t));
    primes[0] = 2;
    primes[1] = 3;

    uint32_t testNum = 5;
    uint32_t cpt = 2;//primes counter
    uint32_t index = 1;

    clock_t wallClock = clock();//Starting timer...

    while(cpt<nbrPrimes)
    {
        for (uint32_t k = 0; k < 2; ++k) {
            //Compute next test number
            testNum = 6*index + (k?1:-1);
            if(isPrime(testNum))
            {
                primes[cpt] = testNum;
                cpt++;
            }
        }
        index++;
    }

    wallClock =  clock() - wallClock;
    printf("wall clock time : %lf\n", (double) wallClock/CLOCKS_PER_SEC);

    FILE *pfile = fopen("testPrimes.txt","w");
    for (int i = 0; i < nbrPrimes; ++i)
       fprintf(pfile, "%d\n", primes[i]);
    fclose(pfile);

    printf("%d -> %d\n",nbrPrimes, primes[nbrPrimes-1]);

    free(primes);
    return 0;
}


bool isPrime(uint32_t num) {
    uint32_t sqrtNum = squareRoot(num);
    for (int i = 2; i <= sqrtNum; ++i) {
        if(!(num % i))
            return false;


    }
    return true;
}


