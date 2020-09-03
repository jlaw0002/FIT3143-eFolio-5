#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
int main(int argc, char* argv[]){
    int i;
    double localSum= 0.0;
    double globalSum = 0.0;
    double piVal;
    struct timespec start, end;
    double time_taken;
    int myrank;
    int numProcessors;
    long N;

    //Intialise MPI and store rank and numProcessors
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);
    
    //Root process prompts user for N
    if (myrank == 0){
        printf("Enter a value for N: ");
        fflush(stdout);
        scanf("%li", &N);
      
     }
    // Get current clock time.
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    //Broadcast the value of N inputed to all processes  
    MPI_Bcast(&N, 1, MPI_LONG, 0, MPI_COMM_WORLD);
  
    
    //Split the work evenly across each process
    int npp= N/numProcessors; //npp = numbers to calculate per process
    int nppr = N % numProcessors; //nppr = num per process remainder
    
    int sp = myrank * npp; // Start point
	int ep = sp + npp; // End point = start point + npp
	
	//Add any remainders to the last thread
	if(myrank == numProcessors-1)
		ep += nppr;
		
	//Calculate sum values 
    for(i = sp; i < ep; i++){
        localSum += 4.0 / (1 + pow((2.0 * i + 1.0)/(2.0 * N), 2));
    }
    
    //Reduce to sum all the values of localSum into globalSum
    MPI_Reduce(&localSum, &globalSum,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
   
    if (myrank==0){
        //Calculate the final value of pi
        piVal = globalSum / (double)N;
        
        // Get the clock current time again
        // Subtract end from start to get the CPU time used.
        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9;
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) *1e-9;
        
        //Print output values and time taken
        printf("Calculated Pi value (Parallel-AlgoI) = %12.9f from rank %d \n",piVal, myrank);
        printf("Overall time (p) using %d processors: %lf\n",numProcessors, time_taken); // tp
    }
    //Exit 
    MPI_Finalize();
    return 0;
}
