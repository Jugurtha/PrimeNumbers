#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <omp.h>

// By using the list of primes from http://www.naturalnumbers.org/primes.html we can approximate the distance between
// two consecutive prime number in average to 16 (with average distance of 10,4729 and average
// absolute deviation of 6,07694858 using a sample of 10000 primes). Using this, we can approximate the value of the
// 1000000th prime to 16000000. So, by using the algorithm in isPrimeWithMem, with tests the prime numbers inferior to
// sqrt(num) we can safely assume that a cache size of sqrt(16000000)=4000 elements will be sufficient.
#define CACHE_SIZE (4000/4)
#define NBR_SMALL_PRIMES 50
#define NBR_THREADS 2


uint32_t squareRoot(uint32_t a_nInput);
bool isPrime(uint32_t num);
bool isPrimeWithMem(uint32_t num, const uint32_t *previousPrimes, uint32_t nbrPreviousPrimes);
uint32_t binarySearchGreaterOrEqual(const uint32_t arr[], uint32_t l, uint32_t r, uint32_t x);



int main(int argc, char *argv[]) {
    printf("Sequential :\n");

    if(!omp_get_cancellation())
    {
        perror("OMP Cancellation disabled.");
        exit(EXIT_FAILURE);
    }

    uint32_t nbrPrimes = strtoul(argv[1],NULL,10);//Getting number of primes to find from stdin at lunch
    printf("Nbr Primes to find : %d\n",nbrPrimes);

    uint32_t *primes = NULL;
    primes = malloc(nbrPrimes* sizeof(uint32_t));
    if(primes==NULL)
    {
        perror("Unable to allocate memory for prime numbers.");
        exit(EXIT_FAILURE);
    }
    primes[0] = 2;
    primes[1] = 3;

    uint32_t testNum;
    uint32_t cpt = 2;//primes counter
    uint32_t index = 1;

    clock_t wallClock = clock();//Starting timer...

    while(cpt<nbrPrimes)
    {
        for (uint32_t k = 0; k < 2 && cpt < nbrPrimes; ++k) {
            //Compute next test number
            testNum = 6*index + (k?1:-1);
            //if(isPrimeWithMem(testNum,primes,cpt))//For the tests using previously memorised primes
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

    //Comparing results with the first 1000000 primes from http://www.naturalnumbers.org/primes.html using WinMerge
    // (this is not portable)
    FILE *pfile = NULL;
    pfile = fopen("../Tests/testPrimes.txt","w");
    if(pfile==NULL)
        perror("Unable to open test output file.");
    else{
        for (int i = 0; i < nbrPrimes; ++i)
            fprintf(pfile, "%d\n", primes[i]);
        fclose(pfile);
        system("C:\\\"Program Files (x86)\"\\WinMerge\\WinMergeU /wl /u /e ..\\Tests\\primes.txt ..\\Tests\\testPrimes.txt");
    }

    printf("%d -> %d\n",nbrPrimes, primes[nbrPrimes-1]);

    free(primes);
    return 0;
}

bool isPrime(uint32_t num) {
    uint32_t sqrtNum = squareRoot(num);
    bool result = true;

    #pragma omp parallel default(none) shared(result) firstprivate(num, sqrtNum) num_threads(NBR_THREADS)
    {
        #pragma omp for schedule(dynamic)
        for (int i = 2; i <= sqrtNum; ++i) {
           // printf("%d -> %d: %d\n", omp_get_thread_num(),  num, i);
            if (!(num % i)) {
                #pragma omp critical
                {
                    result = false;
                }
                #pragma omp cancel for
            }
        }
    }
    return result;
}
//TODO Parallelise the code below using pragma and cancellation points.
bool isPrimeWithMem(uint32_t num, const uint32_t *previousPrimes, uint32_t nbrPreviousPrimes)
{
    uint32_t sqrtNum = squareRoot(num);
    uint32_t limit = nbrPreviousPrimes;


    //Verify if the element's index is cacheed
    static uint32_t cache[CACHE_SIZE] = {0};
    uint32_t cacheIndex = sqrtNum % CACHE_SIZE;

    if(previousPrimes[cache[cacheIndex]] == sqrtNum)//if sqrtNum is a prime that was already cached
        limit = cache[cacheIndex];
    else
    {
        limit = binarySearchGreaterOrEqual(previousPrimes,0,nbrPreviousPrimes,sqrtNum);
        cacheIndex = previousPrimes[limit] % CACHE_SIZE;
        if(!cache[cacheIndex])// we add it to the cache if it's not already cashed
            cache[cacheIndex] = limit;
        else
            if(cacheIndex > NBR_SMALL_PRIMES)// we change it if it's not too small
                cache[cacheIndex] = limit;
    }


    bool result = true;
    for (int i = 0; i <= limit ; ++i)
        if(!(num % previousPrimes[i]))
        {
            result = false;
            break;
        }
//TODO Add cache hit and miss counters and print them.
/*Dumping cache content in file for testing
    if(num == 15485863) {
        FILE *pfile = NULL;
        pfile = fopen("../Tests/cache.txt", "w");
        if (pfile == NULL)
            perror("Unable to open cache output file.");
        else {
            for (int i = 0; i < CACHE_SIZE; ++i)
                fprintf(pfile, "%d: %d\n", i, cache[i]);
            fclose(pfile);
        }
    }
//*/

    return result;
}


