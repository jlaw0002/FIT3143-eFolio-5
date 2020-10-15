
#include "sumFactorials.h"


void
sum_prog_1(char *host,int x)
{   
	CLIENT *clnt;
	long  *result_1;
	number  sumfactorial_1_arg;

    //Create client and server connection
#ifndef	DEBUG
	clnt = clnt_create (host, SUM_PROG, SUM_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */
    //Pass the parameter into the number struct defined in sumFactorials.x
	sumfactorial_1_arg.x=x;
	//Remote process call to server and store result
	result_1 = sumfactorial_1(&sumfactorial_1_arg, clnt);
	
	//Check whether the result succeeded
	if (result_1 == (long *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	else{
	    //Print out result obtained from server
	    printf("Result: %ld \n", *result_1);
	}

#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;
    //Check that program is called properly
	if (argc < 3) {
		printf ("usage: %s ./sumFactorials_client server_host NUMBER \n", argv[0]);
		exit (1);
	}
	//Set host from first parameter
	host = argv[1];
    //Call main program
	sum_prog_1 (host, atoi(argv[2]));

exit (0);
}
