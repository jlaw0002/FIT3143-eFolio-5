#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>  

//Macro defintions 
#define MAX_GENERATED_NUM 25
#define NUMBER_OF_SLOTS 10

// Function Declarations
int* initialiseCounter();
int findWins(int *array);

int main(){
   
    int num, i;
    int results[NUMBER_OF_SLOTS];
    int *resultsArray=results;

    
    /* Intializes random number generator */
    time_t t;
    srand((unsigned) time(&t));
        
    
    //Parallel part of code to generate random numbers
    #pragma omp parallel for private(i, num) shared(resultsArray) schedule(static, 2) num_threads(4)
	    for(i = 0; i < NUMBER_OF_SLOTS; i++){
		    // Call RNG function
		    num = (rand() % MAX_GENERATED_NUM) + 1;
		    resultsArray[i]=num;

	    }
	
	//Call functions to calculate wins and print result
	printf("Number of wins: %d\n", findWins(resultsArray));
	return 0;
	
}

int* initialiseCounter(){
    //Create an array to store occurrences of all possible numbers
    static int array[MAX_GENERATED_NUM];
    
    //Intialise each count to 0
    for(int i = 0; i < MAX_GENERATED_NUM; i++){
        array[i] = 0;
    }

    return array;
}

int findWins(int *resultsArray){
    int* countArray = initialiseCounter();
    int counter = 0;
    int num;
    
    //Count the number of occurrences of each number in resultsArray
    for (int i=0; i<NUMBER_OF_SLOTS;i++){
        num = resultsArray[i];
        countArray[num]+=1;
    }
    
    //Check if occurrences of each number is greater than 1
    for(int i = 0; i < MAX_GENERATED_NUM; i++){
        if(countArray[i] > 1){
            counter++;
        }
    }

    return counter;
}
