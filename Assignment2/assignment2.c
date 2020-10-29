#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1
#define ITERATIONS 10
int base_io(MPI_Comm world_comm, MPI_Comm comm);
int sensor_io(MPI_Comm world_comm, MPI_Comm comm, int* dims);

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

    //Check for command line arguments
    if (argc == 3) {
        nrows = atoi (argv[1]);
        ncols = atoi (argv[2]);
        dims[0] = nrows; /* number of rows */
        dims[1] = ncols; /* number of columns */
        if( (nrows*ncols) != size-1) {
            if( my_rank ==0){
            	printf("ERROR: Number of processes needs to be (nrows*ncols + 1) \n");
                printf("ERROR: nrows*ncols =%d * %d = %d != %d\n", nrows, ncols, nrows*ncols, size-1);
            	printf("Usage: mpirun --oversubscribe -np <nprocesses> assignment2 <nrows> <ncols>\n");
            }
            	
            MPI_Finalize();
            return 0;
        }
        printf("Using user specified dimensions for sensor grid \n");
    } else {
    	//No command line arguments
        nrows=ncols=(int)sqrt(size);
        //Let dimensions be auto generated
        dims[0]=dims[1]=0;
        printf("Auto generating dimensions for sensor grid \n");
    }

	MPI_Comm_split( MPI_COMM_WORLD,my_rank != size-1, 0, &comm2D); 

   	if (my_rank == size-1) 
		base_io( MPI_COMM_WORLD, comm2D );
    else
    	//Need to pass in dims
		sensor_io( MPI_COMM_WORLD, comm2D, dims);
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
int sensor_io(MPI_Comm world_comm, MPI_Comm comm, int* dims){
	int ndims=2, size, myRank, reorder, myCartRank, ierr, worldSize;
	MPI_Comm comm2D;
	int coord[ndims];
	int wrap_around[ndims];
	char buf[256];

	int myTemp;

	//Neighbours
    int nbr_i_lo, nbr_i_hi; //top, bottom
    int nbr_j_lo, nbr_j_hi; //left, right

	MPI_Comm_size(world_comm, &worldSize); // size of the world communicator
  	MPI_Comm_size(comm, &size); // size of the slave communicator
	MPI_Comm_rank(comm, &myRank);  // rank of the slave communicator
	
	printf("Sensor Node: Global Rank %d \n",myRank);

	//dims[0]=dims[1]=0;
	MPI_Dims_create(size, ndims, dims);

	 /* create cartesian mapping */
	wrap_around[0] = 0;
	wrap_around[1] = 0; /* periodic shift is .false. */
	reorder = 0;
	ierr =0;
	ierr = MPI_Cart_create(comm, ndims, dims, wrap_around, reorder, &comm2D);
	if(ierr != 0) printf("ERROR[%d] creating CART\n",ierr);

	if(myRank==0)
		printf("Slave Rank: %d. Comm Size: %d: Grid Dimension = [%d x %d] \n",myRank,size,dims[0],dims[1]);
	
	MPI_Cart_coords(comm2D, myRank, ndims, coord);
	MPI_Cart_rank(comm2D, coord, &myCartRank);

	//Get neighbors of up, down left right
  	MPI_Cart_shift( comm2D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi);
    MPI_Cart_shift( comm2D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi);

	/* Generate and assign a random prime number */
	srand(time(0)+myRank);
    myTemp = rand() % (21); //Generate random number from 0-20

    printf("Global rank: %d. Cart rank: %d. Coord: (%d, %d). Left: %d. Right: %d. Top: %d. Bottom: %d. My value: %d\n", myRank, myCartRank, coord[0], coord[1], nbr_j_lo, nbr_j_hi, nbr_i_lo, nbr_i_hi,myTemp);
	fflush(stdout);


	MPI_Comm_free( &comm2D );
	return 0;
}