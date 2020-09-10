#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h> 

//Function Declarrations
bool isPrime(int n);

int main(int argc, char **argv){
    
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
        scanf(" %d", &n);
          
        // Start timer
        start = clock();


	}
	
	//Broadcast the value of n to all processes
    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
    
    
    //Split the work across each process 
    int npp= n/size; //npt = numbers per process
    int nppr = n % size; //nrpt = num per process remainder

    int sp = rank * npp; // Start point
	int ep = sp + npp; // End point = start point + npp

	//Add remainders to the last process
	if(rank == size-1)
		ep += nppr;
	

    

    //Allocate memory
    if (rank==0){
        //Root process will need all results
        primesArray=(int*)malloc(n * sizeof(int));
    }
    else{
        //Other processes only need memory for numbers they are calculating
        primesArray = (int*)malloc((ep-sp) * sizeof(int));
    }
    
    
    // Parallel computing of prime numbers
    for(int i = sp; i< ep; i++){ 
		if(isPrime(i)){
            primesArray[i-sp] = i;
        }
	}
    
    //Send the arrays results back to the root process
    if (rank==0){
        //Root process only needse to receive data
        int offset;
        offset = npp;
        for (int i=1; i< size; i++){
        
            //Iterate through each process and receive their data sent
            if (i!=size-1){
		        MPI_Recv((int*)primesArray + offset, npp, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            }
            else{
                //Last process will send any remainders so need to change input size
		        MPI_Recv((int*)primesArray + offset, npp+nppr, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
            }
            //Increase offset pointer by amount of numbers per process
		    offset += npp; 
        } 
  
    }
    else{
        //Send data to root process
        fflush(stdout);
        MPI_Send((int*)primesArray, (ep - sp), MPI_INT, 0, 0, MPI_COMM_WORLD);

    }
    
    if (rank ==0){
        //Write prime numbers to file
        FILE *fp;
        fp = fopen("primes_all.txt", "w+");
        fprintf(fp, "Prime Numbers from %d to %d:\n", 0, n);

       
        for (int i = 0; i < n; i++){
            if (primesArray[i])
                fprintf(fp, "%d\n", i);
        }
        
        //Close file and free allocated memory
        fclose(fp);
        printf("%s created\n", "primes_all.txt");
        fflush(stdout);
        
        
        // End timer and print duration
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("This program took %f to execute\n", cpu_time_used);
        
    }
    
    //Cleanup and Exit program 
    free(primesArray);
    MPI_Finalize();
    return 0;
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
