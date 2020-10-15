/* Gets the neighbors in a cartesian communicator
* Orginally written by Mary Thomas
* - Updated Mar, 2015
* Link: https://edoras.sdsu.edu/~mthomas/sp17.605/lectures/MPICart-Comms-and-Topos.pdf
* Minor modifications to fix bugs and to revise print output
*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>
#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1

int generatePrime();
bool isPrime(int n);

int main(int argc, char *argv[]) {
    int ndims=2, size, my_rank, reorder, my_cart_rank, ierr;
    int nrows, ncols;
    int nbr_i_lo, nbr_i_hi;
    int nbr_j_lo, nbr_j_hi;
    MPI_Comm comm2D;
    int dims[ndims],coord[ndims];
    int wrap_around[ndims];
    int myValue;
    
    /* start up initial MPI environment */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    /* process command line arguments*/
    if (argc == 3) {
        nrows = atoi (argv[1]);
        ncols = atoi (argv[2]);
        dims[0] = nrows; /* number of rows */
        dims[1] = ncols; /* number of columns */
        if( (nrows*ncols) != size) {
            if( my_rank ==0)
                printf("ERROR: nrows*ncols)=%d * %d = %d != %d\n", nrows, ncols, nrows*ncols, size);
            MPI_Finalize();
            return 0;
        }
    } else {
        nrows=ncols=(int)sqrt(size);
        dims[0]=dims[1]=0;
    }
   
    /************************************************************
    */
    /* create cartesian topology for processes */
    /************************************************************
    */
    MPI_Dims_create(size, ndims, dims);
    if(my_rank==0)
        printf("Root Rank: %d. Comm Size: %d: Grid Dimension = [%d x %d] \n", my_rank, size, dims[0], dims[1]);
    
    /* create cartesian mapping */
    wrap_around[0] = wrap_around[1] = 0; /* periodic shift is false. */
    reorder = 1;
    ierr =0;
    ierr = MPI_Cart_create(MPI_COMM_WORLD, ndims, dims, wrap_around, reorder, &comm2D);
    
    if(ierr != 0) 
        printf("ERROR[%d] creating CART\n",ierr);
    
    /* find my coordinates in the cartesian communicator group */
    MPI_Cart_coords(comm2D, my_rank, ndims, coord);
    
    /* Generate and assign a random prime number */
    myValue = generatePrime();
    
    /* use my cartesian coordinates to find my rank in cartesian group*/
    MPI_Cart_rank(comm2D, coord, &my_cart_rank);
    /* get my neighbors; axis is coordinate dimension of shift */
    /* axis=0 ==> shift along the rows: P[my_row-1]: P[me] : P[my_row+1] */
    /* axis=1 ==> shift along the columns P[my_col-1]: P[me] : P[my_col+1] */
    MPI_Cart_shift( comm2D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi);
    MPI_Cart_shift( comm2D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi);
    printf("Global rank: %d. Cart rank: %d. Coord: (%d, %d). Left: %d. Right: %d. Top: %d. Bottom: %d. My value: %d\n", my_rank, my_cart_rank, coord[0], coord[1], nbr_j_lo, nbr_j_hi, nbr_i_lo, nbr_i_hi,myValue);
    fflush(stdout);
    
    MPI_Comm_free( &comm2D );
    MPI_Finalize();
    return 0;
}

int generatePrime(){
    // Use current time as seed for random generator
    srand(time(0)); 
    
    int randomNumber;
    bool randomPrimeFound = false;
    
    while(!randomPrimeFound){
        randomNumber = rand();
        if(isPrime(randomNumber)){
            randomPrimeFound = true; 
        }
    }
    
    return randomNumber;
}

/* Code taken from Week 3 Lab */
/* Reference: https://www.geeksforgeeks.org/print-all-prime-numbers-less-than-or-equal-to-n */
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
