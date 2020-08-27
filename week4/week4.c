#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAX_GENERATED_NUM 25
#define NUMBER_OF_SLOTS 10

// Function Declarations
int* initialiseCounter(int num);
int findWins(int *array);

int main(){
   
    int num, i;
    int* resultsArray = initialiseCounter(MAX_GENERATED_NUM);
   
    /* Intializes random number generator */
    time_t t;
    srand((unsigned) time(&t));
        
    
    #pragma omp parallel for private(i, num) shared(resultsArray) schedule(static, 2) num_threads(4)
	    for(i = 0; i < NUMBER_OF_SLOTS; i++){
		    // Call RNG function
		    num = (rand() % MAX_GENERATED_NUM) + 1;
		    resultsArray[num] += 1;
	    }
	
	printf("Number of wins: %d\n", findWins(resultsArray));
	return 0;
	
}

int* initialiseCounter(int num){
    static int array[MAX_GENERATED_NUM];
    
    for(int i = 0; i < num; i++){
        array[i] = 0;
    }

    return array;
}

int findWins(int *array){
    int counter = 0;
    
    for(int i = 0; i < MAX_GENERATED_NUM; i++){
        if(array[i] > 1){
            counter++;
        }
    }

    return counter;
}
