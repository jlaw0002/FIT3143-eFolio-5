#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
int main(int argc, char* argv[]){

    int myrank;
    int numProcessors;
    int N;
    int total;
    //Intialise MPI and store rank and numProcessors
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);
    MPI_Status status;
    //Root process prompts user for N
    if (myrank == 0){
        printf("Processor Rank %d: Enter a value for N: \n", myrank);
        fflush(stdout);
        scanf("%i", &N);
        //Send to process rank 1 for computation
        MPI_Send(&N, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

     }
     
     //Process 1 calculates sum of factorials
     else if (myrank ==1){
        //Receive value of N from root 
        MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        total=0;
        printf("Processor Rank %d was called and received value N of: %d \n",myrank,N);
        //Iterate through each Number
        for (int i=1; i<=N; i++){
            printf("Processor Rank %d: Calculating factorial for %d \n",myrank,i);
            long factorial=1;
            //Calculate Factorial
            for (int j=1; j<=i; j++){
                factorial*=j;        
            }
            //Print factorial just calculated
            printf("Processor Rank %d: Factorial for %d is %ld \n", myrank,i,factorial);
            //Add to total
            total+=factorial;
        }
        //Send result back to root process
        MPI_Send(&total, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
     }
     
    //Root process receives total of computation from Process 1
    if (myrank == 0){

        MPI_Recv(&total, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        printf("Processor Rank %d: Result received from process 1 is: %d \n",myrank,total);
     }
    
    //Exit 
    MPI_Finalize();
    return 0;
}
