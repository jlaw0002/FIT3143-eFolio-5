#include <mpi.h>
#include <pthread.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stddef.h>
#include <unistd.h> 

#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1
#define ITERATIONS 10
#define MIN_TEMP 60
#define MAX_TEMP 100
#define THRESHOLD 80
#define TOLERANCE 5

#define SENSOR_STATUS_NO_ALERT 0
#define SENSOR_STATUS_ALERT 1 

#define CONTINUE 1
#define EXIT_TAG 0

typedef struct {
	int myRank;
    int myTemp;
    int myCoord[2];
    int adjacentRanks[4];
    int adjacentTemps[4];
    int adjacentCoordsX[4];
    int adjacentCoordsY[4];
    char alertTime[50];

    // char macAddrerss[50];
} sensorAlert;

struct satelliteReading {
    int temp;
    int coords[2];
    char time[50];
} ;

struct satelliteReading satelliteReadings[ITERATIONS];
int nreadings = 0;

int base_io(MPI_Comm world_comm, MPI_Comm comm, int* dims);
void* getSatelliteReading(void *pArg);
int sensor_io(MPI_Comm world_comm, MPI_Comm comm, int* dims);

int main(int argc, char *argv[]) {
	int ndims=2, size, myRank, reorder, my_cart_rank, ierr;
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
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    //Check for command line arguments
    if (argc == 3) {
        nrows = atoi (argv[1]);
        ncols = atoi (argv[2]);
        dims[0] = nrows; /* number of rows */
        dims[1] = ncols; /* number of columns */
        //Check that user specified dimensions match up with number of processes 
        if( (nrows*ncols) != size-1) {
            if( myRank ==0){
            	printf("ERROR: Number of processes needs to be (nrows*ncols + 1) \n");
                printf("ERROR: nrows*ncols =%d * %d = %d != %d\n", nrows, ncols, nrows*ncols, size-1);
            	printf("Usage: mpirun --oversubscribe -np <nprocesses> assignment2 <nrows> <ncols>\n");
            }
            	
            MPI_Finalize();
            return 0;
        }
        if(myRank == 0){
        	printf("Using user specified dimensions for sensor grid \n");
        }
        
    } else {
    	//No command line arguments
        nrows=ncols=(int)sqrt(size);

        //Let dimensions be auto generated
        dims[0]=dims[1]=0;
     	if(myRank == 0){
        	printf("Auto generating dimensions for sensor grid \n");
        }
        
    }

	MPI_Comm_split( MPI_COMM_WORLD,myRank != size-1, 0, &comm2D); ;
    
   	if (myRank == size-1) 
		base_io(MPI_COMM_WORLD, comm2D, dims);
    else
		sensor_io(MPI_COMM_WORLD, comm2D, dims);

    
    MPI_Finalize();
    
    return 0;
}

/* This is the master */
int base_io(MPI_Comm world_comm, MPI_Comm comm, int* dims){
	int i, size, nslaves,myRank; 
	int totalAlerts = 0, trueAlerts = 0, falseAlerts = 0;
	char buf[256], buf2[256];
	MPI_Status status;
	MPI_Comm_size(world_comm, &size );
	MPI_Comm_rank(world_comm, &myRank);

	clock_t start, end;
    double commTimeBetweenReporterAndBase;

	//Alert struct
    MPI_Datatype mpiSensorAlertType;
    MPI_Datatype type[8] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT,MPI_INT,MPI_INT,MPI_CHAR};
  	int blocklen[8] = {1,1,2,4,4,4,4,50};
  	MPI_Aint offsets[8];

    offsets[0] = offsetof(sensorAlert, myRank);
    offsets[1] = offsetof(sensorAlert, myTemp);
    offsets[2] = offsetof(sensorAlert, myCoord);
    offsets[3] = offsetof(sensorAlert, adjacentTemps);
    offsets[4] = offsetof(sensorAlert, adjacentRanks);
    offsets[5] = offsetof(sensorAlert, adjacentCoordsX);
    offsets[6] = offsetof(sensorAlert, adjacentCoordsY);
    offsets[7] = offsetof(sensorAlert, alertTime);

    // Create MPI struct
    MPI_Type_create_struct(8, blocklen, offsets, type, &mpiSensorAlertType);
    MPI_Type_commit(&mpiSensorAlertType);

    sensorAlert alert;

	nslaves = size - 1;
	//printf("Base Station Master Node: Global Rank %d \n",myRank);
	
	int messageTracker[nslaves];

	for(int i = 0; i < nslaves; i++){
		messageTracker[i] = 0;
	}

	int testCount=0;
	for (int i=0; i<ITERATIONS; i++){
	    // Infrared Imaging Satellite Simulation using a POSIX thread
        pthread_t tid;

        // for(int j=0; j < sizeof satelliteReadings; j++){
        // 	pthread_create(&tid, 0, getSatelliteReading, (void*) dims); // Create the thread
        // 	pthread_join(tid, NULL); // Wait for the thread to complete.
        // }

        pthread_create(&tid, 0, getSatelliteReading, (void*) dims); // Create the thread
        pthread_join(tid, NULL); // Wait for the thread to complete.
        
 
		// I think each sensor node needs to communicate to the base station.
		// Dont think its possible for the sensor node to send a "done" message as each
		// sensor node would have to communicate with each other.
        
        // Start timer
    	start = clock();

		for (int j=0; j< nslaves; j++){
			//Need to keep in seperate for loop to send first
			MPI_Send(&i, 1, MPI_INT, j, CONTINUE, world_comm);
		}
		
		for (int j=0; j< nslaves; j++){
			//Send status message to sensor node to keep generating numbers
			

			//printf("Looking for message from sensor node with rank %d \n",j);
			MPI_Recv(&alert, 1, mpiSensorAlertType, j, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
			
			// End timer and print duration
    		end = clock();
    
			//printf("messaged received from sensor node with rank %d \n",j);
			if(status.MPI_TAG == SENSOR_STATUS_ALERT){

				commTimeBetweenReporterAndBase = ((double) (end - start)) / CLOCKS_PER_SEC;
			
			    //Get current time for logging
	            time_t currentTime = time(NULL);
	            char * currentTimeString = ctime(&currentTime);
	            currentTimeString[strlen(currentTimeString)-1] = '\0';

	            messageTracker[j]++;
	            totalAlerts++;
				
				/*
				printf("BASE STATION[%d][%d]: Received ALERT %s \n",i,testCount, alert.alertTime);
				printf("BASE STATION[%d][%d]: Received ALERT from sensor node rank %d with temp: %d, tag: %d, coords(%d,%d) \n",i,testCount,alert.myRank,alert.myTemp,status.MPI_TAG,alert.myCoord[0],alert.myCoord[1]);
				printf("BASE STATION[%d][%d]: Adjacent Ranks: [top:%d, down:%d, left:%d, right:%d], Adjacent Temps[%d,%d,%d,%d] \n",i,testCount,alert.adjacentRanks[0],alert.adjacentRanks[1],alert.adjacentRanks[2],alert.adjacentRanks[3],alert.adjacentTemps[0],alert.adjacentTemps[1],alert.adjacentTemps[2],alert.adjacentTemps[3]);
				printf("BASE STATION[%d][%d]: Adjacent Coords: (%d,%d),(%d,%d),(%d,%d),(%d,%d)\n",i,testCount,alert.adjacentCoordsX[0],alert.adjacentCoordsY[0],alert.adjacentCoordsX[1],alert.adjacentCoordsY[1],alert.adjacentCoordsX[2],alert.adjacentCoordsY[2],alert.adjacentCoordsX[3],alert.adjacentCoordsY[3]);
				*/
				
				int flag = 0, adjacentMatches = 0;
				struct satelliteReading flaggedReading;
				
				for(int i = 0; i < nreadings; i++){
					// printf("# Now reading: %d°C at (%d,%d) on %s\n", satelliteReadings[i].temp, satelliteReadings[i].coords[0], satelliteReadings[i].coords[1], satelliteReadings[i].time);

					if((satelliteReadings[i].coords[0] == alert.myCoord[0]) && (satelliteReadings[i].coords[1] == alert.myCoord[1]) && (abs(satelliteReadings[i].temp - alert.myTemp) <= TOLERANCE)){
						flag = 1;
						flaggedReading.temp = satelliteReadings[i].temp;
				        memcpy(flaggedReading.coords, satelliteReadings[i].coords, sizeof satelliteReadings[i].coords);
				        strcpy(flaggedReading.time, satelliteReadings[i].time);
					} else {
						for(int j = 0; j < sizeof alert.adjacentCoordsX; j++){
							if(satelliteReadings[i].coords[0] == alert.adjacentCoordsX[j] && satelliteReadings[i].coords[1] == alert.adjacentCoordsY[j]){
								if((alert.adjacentTemps[j] > 0) && (abs(satelliteReadings[i].temp - alert.adjacentTemps[j]) <= TOLERANCE)){
									flag = 1;
									flaggedReading.temp = satelliteReadings[i].temp;
							        memcpy(flaggedReading.coords, satelliteReadings[i].coords, sizeof satelliteReadings[i].coords);
							        strcpy(flaggedReading.time, satelliteReadings[i].time);
							    }
							}
						}
					}	        
				}

				for(int j = 0; j < sizeof alert.adjacentTemps; j++){
					if((alert.adjacentTemps[j] > 0) && abs(alert.myTemp - alert.adjacentTemps[j]) <= TOLERANCE)
						adjacentMatches++;
				}
				
				printf("--------------------------------------------------\n");
				printf("Iteration: %d\n", i);
				printf("Logged Time:\t\t%s\n", currentTimeString);
				printf("Alert Reported Time:\t%s\n", alert.alertTime);
				 
				if(flag == 1){
				   printf("Alert Type: True\n\n");
				   trueAlerts++;
				}
				else{
				   printf("Alert Type: False\n\n");
				   falseAlerts++;
				}
				   
				printf("Reporting Node\tCoord\tTemp\n");
				printf("%d\t\t(%d,%d)\t%d°C\n\n", alert.myRank, alert.myCoord[0], alert.myCoord[1], alert.myTemp);
				
				printf("Adjacent Nodes\tCoord\tTemp\n");
				for(int k = 0; k < 4; k++){
					if(alert.adjacentTemps[k] > 0)
				    	printf("%d\t\t(%d,%d)\t%d°C\n", alert.adjacentRanks[k], alert.adjacentCoordsX[k], alert.adjacentCoordsY[k], alert.adjacentTemps[k]);
				}

				printf("\n");
				
				if(flag == 1){
					printf("Infrared Satellite Reporting Time: %s\n", flaggedReading.time);
					printf("Infrared Satellite Reporting Temp: %d°C\n", flaggedReading.temp);
					printf("Infrared Satellite Reporting Coord: (%d,%d)\n\n", flaggedReading.coords[0], flaggedReading.coords[1]);
				}
				
				//printf("Communication Time bbetween adjacent nodes: %fs\n", alert.commTimeBetweenAdjNodes);
				printf("Communication Time between the reporting node and the base station: %fs\n", commTimeBetweenReporterAndBase);
				printf("Total Messages send between reporting node and base station: %d\n", messageTracker[j]);
				printf("Number of adjacent matches to reporting node: %d\n", adjacentMatches);
				printf("--------------------------------------------------\n");
				//fflush(stdout);
			}
			
			testCount++;
		}
		
		//Sleep delay
		//printf("Waiting for delay before next iteration...\n");
		sleep(1);
	}
    
	//Exit message tag
	for (int j=0; j< nslaves; j++){
		MPI_Send(&j, 1, MPI_INT, j, EXIT_TAG, world_comm);
	}
	
	//printf("TEST COUNT : %d \n",testCount);
	printf("\n--------------------------------------------------\n");
	printf("Summary\n");
	printf("True Alerts: %d\n", trueAlerts);
	printf("False Alerts: %d\n", falseAlerts);
	printf("Total Alerts: %d\n", totalAlerts);
	printf("--------------------------------------------------\n");
	fflush(stdout);

	return 0;
}

void *getSatelliteReading(void* pArg) { 
	for(int i = 0; i < ITERATIONS; i++){
	    // Seed RNG with current time 
	    srand(time(NULL) + i);
	    
	    // Generate a (valid) temperature reading
		int tempReading = rand() % (MAX_TEMP + 1 - MIN_TEMP) + MIN_TEMP; //Generate random number from 60-100
		
		// Get sensor network's dimensions from inout parameter
	    int* coords = (int*) pArg;
	    int coordReading[sizeof coords];
	    // Generate random (valid) coordinates
	    coordReading[0] = rand() % coords[0];
	    coordReading[1] = rand() % coords[1];
	    
	    //Get current time for reading
		time_t currentTime = time(NULL);
		char * currentTimeString = ctime(&currentTime);
		currentTimeString[strlen(currentTimeString)-1] = '\0';
				    
	    //printf("# Temperature of %d at (%d, %d) at %s\n", tempReading, coordReading[0], coordReading[1], currentTimeString);
	    
	    // Create reading
	    struct satelliteReading reading;
	    reading.temp = tempReading;
	    memcpy(reading.coords,coordReading, sizeof coordReading);
	    strcpy(reading.time,currentTimeString);
	    
	    // Add reading to global variable so that base node can use reading for analysis
	    // if(nreadings >= (ITERATIONS - 1))
	    //     nreadings = 0;
	        
	    satelliteReadings[i] = reading;
	    //nreadings++;
	    nreadings = i;
	}
}

int sensor_io(MPI_Comm world_comm, MPI_Comm comm, int* dims){
	int ndims=2, size, myRank, reorder, myCartRank, ierr, worldSize;
	MPI_Comm comm2D;
	int coord[ndims];
	int wrap_around[ndims];
	char buf[256];
	MPI_Status status;
	int myTemp,sensorStatus;
	int nAdjacent=4;

	clock_t start, end;
    double commTimeBetweenAdjNodes;

	//Alert struct
    MPI_Datatype mpiSensorAlertType;
    MPI_Datatype type[8] = { MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT,MPI_INT,MPI_INT,MPI_CHAR};
  	int blocklen[8] = {1,1,2,4,4,4,4,50};
  	MPI_Aint offsets[8];

    offsets[0] = offsetof(sensorAlert, myRank);
    offsets[1] = offsetof(sensorAlert, myTemp);
    offsets[2] = offsetof(sensorAlert, myCoord);
    offsets[3] = offsetof(sensorAlert, adjacentTemps);
    offsets[4] = offsetof(sensorAlert, adjacentRanks);
    offsets[5] = offsetof(sensorAlert, adjacentCoordsX);
    offsets[6] = offsetof(sensorAlert, adjacentCoordsY);
    offsets[7] = offsetof(sensorAlert, alertTime);

    // Create MPI struct
    MPI_Type_create_struct(8, blocklen, offsets, type, &mpiSensorAlertType);
    MPI_Type_commit(&mpiSensorAlertType);

    //Assign rank and size variables
	MPI_Comm_size(world_comm, &worldSize); // size of the world communicator
  	MPI_Comm_size(comm, &size); // size of the slave communicator
	MPI_Comm_rank(comm, &myRank);  // rank of the slave communicator
	
	//Create dimensions
	MPI_Dims_create(size, ndims, dims);

	//Create cartesian mapping 
	wrap_around[0] = 0;
	wrap_around[1] = 0; /* periodic shift is .false. */
	reorder = 0;
	ierr =0;
	ierr = MPI_Cart_create(comm, ndims, dims, wrap_around, reorder, &comm2D);
	if(ierr != 0) printf("ERROR[%d] creating CART\n",ierr);

    /*
	if(myRank==0){
		printf("Slave Rank: %d. Comm Size: %d: Grid Dimension = [%d x %d] \n",myRank,size,dims[0],dims[1]);
		fflush(stdout);
	}*/
		
	//Get coordinate and rank
	MPI_Cart_coords(comm2D, myRank, ndims, coord);
	MPI_Cart_rank(comm2D, coord, &myCartRank);

	//Array to store adjacent cart ranks
    // Index: 0=Top 1=Bottom 2=Left 3=Right
	int adjacentCartRanks[nAdjacent];

	//Get neighbors of up, down left right
  	MPI_Cart_shift( comm2D, SHIFT_ROW, DISP, &adjacentCartRanks[0], &adjacentCartRanks[1]);
    MPI_Cart_shift( comm2D, SHIFT_COL, DISP, &adjacentCartRanks[2], &adjacentCartRanks[3]);

    //Arrays to store requests
    MPI_Request send_request[4];
    MPI_Request receive_request[4];
    MPI_Status send_status[4];
    MPI_Status receive_status[4];
    
   	//printf("Rank %d adjacent up: %d ,down %d, left %d, right %d \n",myRank,adjacentCartRanks[0],adjacentCartRanks[1],adjacentCartRanks[2],adjacentCartRanks[3]);


    //First message from base station
    MPI_Recv(&sensorStatus, 1, MPI_INT, worldSize-1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    
    int iterationCount=0;
    //Run until received tag from base station is exit
    while(status.MPI_TAG!=EXIT_TAG){
			
    	//Generate a random temperature
		srand(time(NULL)+myRank*iterationCount);
	    myTemp = rand() % (MAX_TEMP + 1 - MIN_TEMP) + MIN_TEMP; //Generate random number from 60-100

	    // Start timer
    	start = clock();

	    //Send value to all adjacent node
	    for (int i= 0; i< nAdjacent; i++){
	    	
    		MPI_Isend(&myTemp, 1, MPI_INT, adjacentCartRanks[i], 5, comm2D, &send_request[i]);		
	    }
	    
	    //Array to store receivedValues
    	int recvValues[nAdjacent];  // Index: 0=Top 1=Bottom 2=Left 3=Right
    	//Receive value from adjacent nodes
	    for (int i= 0; i< nAdjacent; i++){
    		MPI_Irecv(&recvValues[i], 1, MPI_INT, adjacentCartRanks[i], 5, comm2D, &receive_request[i]);
	    }


      	MPI_Waitall(4, send_request, send_status);	
    	MPI_Waitall(4, receive_request, receive_status);

    	// Start timer
    	end = clock();

	    //Set alert struct
      	sensorAlert alert;
    	alert.myTemp = myTemp;
    	alert.myRank = myRank;
    	alert.myCoord[0] = coord[0];
    	alert.myCoord[1] = coord[1];


	    //Check for possible events
	    if(myTemp > THRESHOLD){
	    	
	    	//Compare between neighbours
	    	int matches = 0;
    	
	    	//Check if received values are in tolerance range
	    	for (int i= 0; i< nAdjacent; i++){

    			//Check if neighbour exists
	    		if (adjacentCartRanks[i]<0){
	    			//Set received value to -1 to ignore
	    			recvValues[i]=-1;
	    		}
	    	
		        if (recvValues[i]<= myTemp+TOLERANCE && recvValues[i]>= myTemp-TOLERANCE){
		        	matches++;
		        }
		    }

		    //Check if more than 2 neighbours have matches
		    if (matches>=2){
		    	//printf("SENSOR NODE[%d]Found alert at rank %d (%d) for mytemp %d with top: %d, bottom: %d, left: %d,right: %d  \n",iterationCount,alert.myRank,myRank,alert.myTemp,recvValues[0],recvValues[1],recvValues[2],recvValues[3]);
		    	//fflush(stdout);
		    	
		    	//Set remaining alert details
	    		for(int i=0; i<nAdjacent; i++){
		    		alert.adjacentRanks[i] = adjacentCartRanks[i];
		    		alert.adjacentTemps[i] = recvValues[i];

		    		//Set adjacentCoords
		    		int adjacentCoordsTemp[2];
		    		if(adjacentCartRanks[i]>=0){	    		
		    			//Adjacent rank exist, get coords
			    		MPI_Cart_coords(comm2D, adjacentCartRanks[i], ndims, adjacentCoordsTemp);	    		
		    		}
		    		else{
		    			//Doesnt exist set coord to -1
		    			adjacentCoordsTemp[0]=-1;
		    			adjacentCoordsTemp[1]=-1;
		    		}
		    		alert.adjacentCoordsX[i]=adjacentCoordsTemp[0];
			    	alert.adjacentCoordsY[i]=adjacentCoordsTemp[1];
		    		
		    	}

		    	//Get current time for alert
		    	time_t currentTime = time(NULL);
			    char * currentTimeString = ctime(&currentTime);
			    currentTimeString[strlen(currentTimeString)-1] = '\0';
			    strcpy(alert.alertTime,currentTimeString);

			    // Communication time between adjacent nodes
			    commTimeBetweenAdjNodes = ((double) (end - start)) / CLOCKS_PER_SEC;
			   
			    
		    	//Send alert
		    	MPI_Send(&alert, 1, mpiSensorAlertType, worldSize-1, SENSOR_STATUS_ALERT, world_comm);
		    }
		    //Send no alert tag to base station
		    else{
		    	//No alert
		    	MPI_Send(&alert, 1, mpiSensorAlertType, worldSize-1, SENSOR_STATUS_NO_ALERT, world_comm);
		    }

	    }
	    else{
	    	//No alert
		    MPI_Send(&alert, 1, mpiSensorAlertType, worldSize-1, SENSOR_STATUS_NO_ALERT, world_comm);

	    }

	
		//Receive next message from base station to check for EXIT_TAG
		MPI_Recv(&sensorStatus, 1, MPI_INT, worldSize-1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		iterationCount++;
    }

    //printf("EXIT_TAG RECEIVED FOR SENSOR %d \n",myRank);

	MPI_Comm_free( &comm2D );
	return 0;						
}																																												
