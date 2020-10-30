#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h> 
#include <mpi.h>
#include <omp.h>

void func();

int main(int argc, char *argv[]) {
  //int size, rank;
  int iam = 0;

  // MPI_Init(&argc, &argv);
  // MPI_Comm_size(MPI_COMM_WORLD, &size);
  // MPI_Comm_rank(MPI_COMM_WORLD, &rank);

omp_set_num_threads(2);

#pragma omp parallel default(shared) private(iam)
  {
    iam = omp_get_thread_num();
    //printf("Hello from thread %d, process %d out of %d\n", iam, rank, size);

    if(iam == 0){
      printf("Generate satellite data\n");
      //printf("# Hello from thread %d, process %d out of %d\n", iam, rank, size);
    } else {
      //printf("Perform base node operations or sensor network based on rank\n");
      //printf("## Hello from thread %d, process %d out of %d\n", iam, rank, size);
      func(argc, argv);
    }

    //printf("Generate satellite data\n");
    //printf("Perform base node operations or sensor network based on rank\n");
  }
  
}


void func(int argc, char *argv[]){
  int size, rank;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  printf("## Hello from process %d out of %d\n", rank, size);

  MPI_Finalize();
}