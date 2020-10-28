#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1
int base_io(MPI_Comm world_comm, MPI_Comm comm);
int sensor_io(MPI_Comm world_comm, MPI_Comm comm);

int main(int argc, char *argv[]) {
	int ndims=2, size, my_rank, reorder, my_cart_rank, ierr;
	int nrows, ncols;
	int nbr_i_lo, nbr_i_hi;
    int nbr_j_lo, nbr_j_hi;
    MPI_Comm comm2D;
    int dims[ndims],coord[ndims];
    int wrap_around[ndims];
    int myValue;
    int nAdjacent=4;

    /* start up initial MPI environment */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_split( MPI_COMM_WORLD,my_rank != size-1, 0, &comm2D); 

   	if (my_rank == size-1) 
		base_io( MPI_COMM_WORLD, comm2D );
    else
		sensor_io( MPI_COMM_WORLD, comm2D );
    MPI_Finalize();
    

    return 0;

}
/* This is the master */
int base_io(MPI_Comm world_comm, MPI_Comm comm){
	int i, size, nslaves, firstmsg,my_rank;
	char buf[256], buf2[256];
	MPI_Status status;
	MPI_Comm_size(world_comm, &size );
	MPI_Comm_rank(world_comm, &my_rank); 
	nslaves = size - 1;
	printf("Base Master Node: Global Rank %d \n",my_rank);

	return 0;
}
int sensor_io(MPI_Comm world_comm, MPI_Comm comm){
	int ndims=2, size, my_rank, reorder, my_cart_rank, ierr, worldSize;
	MPI_Comm comm2D;
	int dims[ndims],coord[ndims];
	int wrap_around[ndims];
	char buf[256];
	MPI_Comm_rank(comm, &my_rank); 
	printf("Sensor Node: Global Rank %d \n",my_rank);
	return 0;
}