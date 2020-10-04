
#include "sumFactorials.h"

long *
sumfactorial_1_svc(number *argp, struct svc_req *rqstp)
{
	static long  result;
    long total=0;
    //Receive value of N from client 
	printf("SERVER: sumFactorial(%d) was called \n",argp->x);
	
    //Iterate through each Number
    for (int i=1; i<=argp->x; i++){
        printf("Calculating factorial for %d \n",i);
        long factorial=1;
        //Calculate Factorial   
        for (int j=1; j<=i; j++){
            factorial*=j;        
        }
        //Print factorial just calculated
        printf("Factorial for %d is %ld \n", i,factorial);
        //Add to total
        total+=factorial;
    }
    //Send result back to client process
    result = total;
	return &result;
}
