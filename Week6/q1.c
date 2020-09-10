#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv){
    int rank, s_value, r_value, size;
    MPI_Status status;
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    
    do {
        if (rank == 0) {
            printf("Enter a round number: ");
            fflush(stdout);
            scanf("%d", &s_value );
            // Add your code here
            printf( "Process %d got %d from Process %d\n", rank, r_value, size - 1);
            fflush(stdout);
        }
        else {
            // Add your code here
            printf("Process %d got %d from Process %d\n", rank, r_value, rank - 1);
            fflush(stdout);
        }
    } while (r_value >= 0);
    
    MPI_Finalize( );
    return 0;
}
