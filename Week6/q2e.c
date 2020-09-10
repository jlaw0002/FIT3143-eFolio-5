#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h> 

//Function Declarrations
bool isPrime(int n);
void threadFunc(int rank, int size, int number);

int main(int argc, char **argv){
    // 
    int rank, size, n, r_value;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Initialise array that will contain prime numbers found
    int *primesArray= NULL;

    // Initialise variables for timing program runtime
    clock_t start, end;
    double cpu_time_used;
    
	if (rank == 0){
	    // Read user input and store it in variable 'n'
        printf("Enter a number: ");
        fflush(stdout);
        scanf("%d", &n);
        
        primesArray = (int*)malloc(n * sizeof(int));
        
        // Start timer
        start = clock();
        
        MPI_Send(&n, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        
	    threadFunc(rank, size, n);
	} else {
	    if(rank != size - 1){
	        MPI_Recv(&r_value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
	        MPI_Send(&r_value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
	        threadFunc(rank, size, r_value);
	    } else {
	        MPI_Recv(&r_value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
	        threadFunc(rank, size, r_value);
	    }
	}
    
    // End timer and print duration
    MPI_Finalize();
    if (rank == 0){ 
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("This program took %f to execute\n", cpu_time_used);
    }
    return 0;
}

void threadFunc(int rank, int size, int number){
    //Split the work across each thread
    int npt= number/size; //npt = numbers per thread
    int nptr = number % size; //nrpt = num per thread remainder

    int sp = rank * npt; // Start point
	int ep = sp + npt; // End point = start point + npt

	//Add remainders to the first thread
	if(rank == size-1)
		ep += nptr;
    
    // Shared memory parallelism
    for(int i = sp; i< ep; i++){ 
		if(isPrime(i)){
            array[i] = i;
        }
	}
	
	char filename[100];
	sprintf(filename, "primes_%d.txt", rank);
	
    FILE *fp;
    fp = fopen(filename, "w+");
    fprintf(fp, "Prime Numbers from %d to %d:\n", sp, ep);
    
    //Write prime numbers to file
    for (int i = sp; i < ep; i++){
        if (array[i])
            fprintf(fp, "%d\n", i);
    }
    
    //Close file and free allocated memory
    fclose(fp);
    printf("%s created\n", filename);
    fflush(stdout);
    free(array);
    return;
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
