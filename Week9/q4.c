#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int master_io(MPI_Comm master_comm, MPI_Comm comm);
int slave_io(MPI_Comm master_comm, MPI_Comm comm, int tag);

int main(int argc, char **argv){
    int rank;
    MPI_Comm new_comm;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_split( MPI_COMM_WORLD,rank == 0, 0, &new_comm);
    if (rank == 0)
        master_io( MPI_COMM_WORLD, new_comm );
    else{
        //Tag 0: Unordered  
        //Tag 1: Ordered      
        //Tag 2: Exit    
        // slave_io( MPI_COMM_WORLD, new_comm, 0);
        slave_io( MPI_COMM_WORLD, new_comm, 1);
        // wait(100);
         // slave_io( MPI_COMM_WORLD, new_comm, 2);
    }

  

    MPI_Finalize();
    return 0;
}

/* This is the master */
int master_io(MPI_Comm master_comm, MPI_Comm comm){
    int i,j, size;
    char buf[256];
    MPI_Status status;
    int activeSlaves;

    MPI_Comm_size( master_comm, &size );
    activeSlaves=size-1;

    while (activeSlaves>0){
        MPI_Recv( buf, 256, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, master_comm, &status );

        switch (status.MPI_TAG) {

            //Tag 0: Unordered  
            case 0:

                for (int i=1; i<(size);i++){
                    MPI_Recv( buf, 256, MPI_CHAR, i, 0, master_comm, &status);
                    fputs( buf, stdout );

                    MPI_Recv( buf, 256, MPI_CHAR, i, 0, master_comm, &status);
                    fputs( buf, stdout );
                }
                
                break;

            //Tag 1: Ordered      
            case 1:
                for (j=1; j<=2; j++) {
                    for (i=1; i<size; i++) {
                        MPI_Recv( buf, 256, MPI_CHAR, i, 1, master_comm, &status);
                        fputs( buf, stdout );
                    }
                }
                break;

            //Tag 2: Exit    
            case 2:
                for (int i=1; i<(size);i++){
                    MPI_Recv( buf, 256, MPI_CHAR, i, 2, master_comm, &status);                                             
                    fputs( buf, stdout );
                    activeSlaves-=1;
                }


                // printf("active slaves: %d \n",activeSlaves);
                break;
        
         }
    }
    
    return 0;
}
    
/* This is the slave */
int slave_io(MPI_Comm master_comm, MPI_Comm comm, int tag){
    char buf[256];
    int rank;
    MPI_Comm_rank(comm, &rank);

    // sprintf(buf, "Sending using tag: %d\n", tag);
    // MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, tag, master_comm ); 

    if (tag!=2){
        sprintf(buf, "Hello from slave %d\n", rank);
        MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, tag, master_comm );

        sprintf( buf, "Goodbye from slave %d\n", rank );
        MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, tag, master_comm );
        return 0;

    }
    else{

        sprintf(buf, "I'm exiting (%d)\n", rank);
        MPI_Send( buf, strlen(buf) + 1, MPI_CHAR, 0, tag, master_comm );
        return 0;

    }
    
}
