#include<stdio.h>
#include<math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h> 

#define NUM_THREADS 4

//Global Variables
int n;
int *primesArray= NULL;

//Function Declarrations
bool isPrime(int n);
void *ThreadFunc(void *pArg);


int main(){
    // Initialise variables for timing program runtime
    clock_t start, end;
    double cpu_time_used;

    // Start timer
    start = clock();
 
    
    // Read user input and store it in variable 'n'
    printf("Enter a number: ");
    scanf("%d", &n);

    
    //Allocate variables and memory for threads and primesArray
    pthread_t tid[NUM_THREADS];
	int threadNum[NUM_THREADS];
    primesArray = (int*)malloc(n * sizeof(int));
    printf("Number of threads: %d \n",NUM_THREADS);
    
    // Create a file named "primes.txt"
    FILE *fp;
    fp = fopen("primes.txt", "w+");
    fprintf(fp, "Prime Number less than %d:\n",n);

    // Fork		
	for (int i = 0; i < NUM_THREADS; i++){
	    threadNum[i] = i;
		pthread_create(&tid[i], 0, ThreadFunc, &threadNum[i]);
	}
	
	// Join
	for(int i = 0; i < NUM_THREADS; i++){
	    	pthread_join(tid[i], NULL);
	}

    //Write prime numbers to file
    for (int i=0; i<n; i++){
        if (primesArray[i])
            fprintf(fp, "%d\n", i);
    }
    //Close file and free allocated memory
    fclose(fp);
    printf("primes.txt created\n");
    free(primesArray);
    
    // End timer and print duration
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("This program took %f to execute\n", cpu_time_used);
    return 0;
}

void *ThreadFunc(void *pArg){
    //Split the work across each thread
    int rank =*((int*)pArg);
    int npt= n/NUM_THREADS; //npt = numbers per thread
    int nptr = n % NUM_THREADS; //nrpt = num per thread remainder

    int sp = rank * npt; // Start point
	int ep = sp + npt; // End point = start point + npt

	//Add remainders to the first thread
	if(rank == NUM_THREADS-1)
		ep += nptr;
    // Shared memory parallelism
    for(int i = sp; i< ep; i++){ 
		if(isPrime(i)){
            primesArray[i]=i;
        }
	}
}

// Reference: https://www.geeksforgeeks.org/print-all-prime-numbers-less-than-or-equal-to-n/
bool isPrime(int n){
    // Edge cases
    if (n <= 1)
        return false;
    if (n <= 3)
        return true;

    // An optimisation to reduce the number of iterations in the 'for' loop
    if(n % 2 == 0 || n % 3 == 0)
        return false;

    for(int i = 5; i <= sqrt(n); i = i + 6){
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }

    return true;
}
