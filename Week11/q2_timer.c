// MPI MergeSort
//************************************************************************************************************
// Note: Running this program with the array size of 1000 in MPICH2 wrapper will cause an error
// The problem is solved by not printing the 1000 unsorted data
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

// Define the size of data
#define N 100000

// Function Prototype
void mergeSort(int* data, int startPoint, int endPoint); 
void merge(int *A, int sizeA, int *B, int sizeB);

// Main Function
int main(int argc, char* argv[])
{
	// Variable declaration
	int i;
	int *data = NULL;	// Initialize data pointer to null
	int scale = 0;
	int currentLevel = 0;	// current level of tree
	int maxLevel = 0;	// maximum level of tree = LOG2 (number of Processors)

	int pivot = 0;		// middle point of data array
	int length = 0;		// length of data array
	int rightLength = 0;	// length of child node data array

	int p;		// Number of Processors
	int myRank;	// Processor's rank
	MPI_Status status;

	FILE *pOutfile;
	double start, end; //timer 
	double cpu_time_used;	

	

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&p);
	MPI_Comm_rank(MPI_COMM_WORLD,&myRank);

	// Start timer
    MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
	start = MPI_Wtime();
	
	if(myRank == 0){
		printf("Starting Program\n");							
		// Root Node:
		maxLevel = log ((double)p) / log(2.00);	// Calculate the maximum level of binary tree
		length = N;	// Set the length of root node to N
		data = (int*)malloc(length * sizeof(int)); // Create dynamic array buffer with size length	
		
		// srand is not used to keep a constant set of random values at each program execution for better debugging
		for(i = 0; i<N;i++)							
			data[i] = rand()%100;
		printf("Finished generating %d numbers in array \n", N);
		// printf("\n-----------------------------------------------------------------------------\n");
		// printf("Unsorted Data: \n");
		// for(i = 0; i<N; i++){	// Prints out unsorted data value
		// 	if(i%10 == 0)	// 10 elements in a row
		// 		printf("\n");
		// 	printf("%d\t",data[i]);
		// }
		// printf("\n-----------------------------------------------------------------------------\n");
	}

	// Broadcast maxLevel to all other processors
	MPI_Bcast(&maxLevel, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	// Divide the data to child node
	for(currentLevel = 0; currentLevel <=maxLevel; currentLevel++){
		scale = pow(2.00, currentLevel);
		
		// Parent node
		if(myRank/scale <1){
			if((myRank+scale)<p){				// if child node exist (child node rank < number of processors)
				pivot = length / 2;					// Divide data length into half
				rightLength = length - pivot;		// Set data length for child node 
				length = pivot;						// Set new data length for parent node
				
				// Send child node length to the corresponding child node
				// tag = currentLevel
				MPI_Send(&rightLength,1,MPI_INT, myRank+scale,currentLevel,MPI_COMM_WORLD); 
				
				// Send the right half of data array
				MPI_Send((int *) data+pivot,rightLength,MPI_INT,myRank+scale,currentLevel,MPI_COMM_WORLD);
			}
		}
		// Child node
		else if(myRank/scale < 2){
			// Receive length from parent node
			MPI_Recv(&length, 1, MPI_INT, myRank-scale,currentLevel, MPI_COMM_WORLD, &status); //tag = currentLevel
			
			// Create new dynamic data buffer with length received from parent
			data = (int*)malloc(length * sizeof(int));
			
			// Receive data array from parent
			MPI_Recv(data, length, MPI_INT,myRank-scale, currentLevel, MPI_COMM_WORLD,&status);
		}
	}
	
	// All processors mergeSort their own data chunk with respective length
	mergeSort(data,0,length -1);		

	// Merge the sorted data from child node to the root node starting 
	// Begin the progress from the lowest level of the tree structure
	for(currentLevel = maxLevel;currentLevel>=0;currentLevel--){
		scale = pow(2.00, currentLevel);
		if(myRank/scale<1){							// Parent node receive sorted data from child node
			if(myRank+scale<p)						// If child node exist (child node rank < number of processors)
			{						
				MPI_Recv(&rightLength, 1, MPI_INT, myRank+scale, currentLevel, MPI_COMM_WORLD,&status);
				MPI_Recv((int *) data+ length, rightLength, MPI_INT, myRank+scale, currentLevel, MPI_COMM_WORLD, &status);

				merge(data, length, (int *)data+length, rightLength);	// Merge the data array
				length+=rightLength; // Update the length of merged data array
			}
		}
		// Child node sends sorted data to parent node
		// tag = current level
		else if(myRank/scale<2)
		{
			// Send the length of sorted data
			MPI_Send(&length, 1, MPI_INT, myRank-scale,currentLevel, MPI_COMM_WORLD);
			MPI_Send(data, length, MPI_INT, myRank-scale, currentLevel, MPI_COMM_WORLD);
		}
	}
	
	// Root node writes the sorted data
	pOutfile = fopen("sorted_parallel.txt","w+");
	if(myRank == 0){
		printf("Writing to file after sorting using Mergesort:\n");
		for(i = 0; i<length;i++){
			if(i%10 == 0)
				fprintf(pOutfile,"\n");
			fprintf(pOutfile,"%d\t", data[i]);
		}
		printf("\n");
	}

	MPI_Barrier(MPI_COMM_WORLD); /* IMPORTANT */
	end = MPI_Wtime();

	MPI_Finalize(); // Finalize MPI
	free(data);	// Free the data buffer

	// End timer and print duration
  	if(myRank == 0){
  		cpu_time_used = (double) (end - start);
    	printf("This program took %f to execute\n", cpu_time_used);
  	}
    

	return 0;
}
// End of main program

// Function mergeSort
void mergeSort(int* data, int startPoint, int endPoint)
{
	int pivot = (startPoint + endPoint)/2;
	
	// if last element then return
	if(startPoint == endPoint)
		return;
	//Recursive function call
	mergeSort(data, startPoint, pivot);
	mergeSort(data,pivot+1,endPoint);
	
	// Merge the sorted data from both sides into the data buffer
	merge(data+startPoint, pivot - startPoint +1, data+pivot+1, endPoint-pivot);
}
// End of function mergeSort

// Function merge
void merge(int *A, int sizeA, int *B, int sizeB)
{
	int sizeC = sizeA + sizeB;
	int *C = (int*)malloc(sizeC * sizeof(int));
	int countA;
	int countB;
	int countC;
	// Merging the element from array A and array B into C in ascending order
	for(countA = 0, countB = 0, countC = 0; countC< sizeC; countC++){
		if(countA>=sizeA)					// If all the element from A is stored into C
			C[countC] = B[countB++];	// store the remaining element from B into C
		else if (countB>=sizeB)			// If all the element from B is stored into C
			C[countC] = A[countA++];		// store the remaining element from  A into C
		else
		{	// Store the element with smaller value into C then increment the corresponding pointer array (A or B)
			if(A[countA]<=B[countB])			
				C[countC] = A[countA++];
			else C[countC] = B[countB++];
		}
	}
	// Copy the merged data from C into A and B 
	for(countA = 0; countA < sizeA; countA++)
		A[countA] = C[countA];
	
	for(countC = countA, countB = 0; countC < sizeC; countC++, countB++)
		B[countB] = C[countC];
	
	// Free memory of C
	free(C);
}
// End of function merge