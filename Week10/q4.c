#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define MSG_EXIT 1
#define MSG_REAL_SOLUTIONS 2
#define MSG_COMPLEX_SOLUTIONS 3
#define MSG_ONE_REAL_SOLUTION 4

int main(int argc, char *argv[]){
    FILE *pInfile,*pOutFile;
    float a_coeff, b_coeff, c_coeff, x1, x2, disc;
    float x1r, x1i, x2r, x2i;
    int fileElementCount = 0, constNeg = -1;
    int counter = 0;
    
    int my_rank;
    int p;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
   

    
    switch (my_rank){
        case 0:{
            //Open file
            pInfile = fopen("quad.txt","r");

            //Get element count
            fscanf(pInfile, "%d", &fileElementCount);
           
            //Skip header line
            fscanf(pInfile, "%*[^\n]\n");

            //Send the counter to the last process
			MPI_Send(&fileElementCount, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);

			// Read each element from the file
			while(counter < fileElementCount)
			{          
				fscanf(pInfile, "%f\t%f\t%f", &a_coeff, &b_coeff, &c_coeff);
				disc = (b_coeff*b_coeff) - 4*a_coeff*c_coeff;
				MPI_Send(&disc, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
				MPI_Send(&a_coeff, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
				MPI_Send(&b_coeff, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
				counter++;
                
			}
			//Close file
			fclose(pInfile);
			pInfile = NULL;
            //Send exit message
			MPI_Send(&disc, 1, MPI_FLOAT, 1, MSG_EXIT, MPI_COMM_WORLD);
			MPI_Send(&a_coeff, 1, MPI_FLOAT, 1, MSG_EXIT, MPI_COMM_WORLD);
			MPI_Send(&b_coeff, 1, MPI_FLOAT, 1, MSG_EXIT, MPI_COMM_WORLD);
            break;

        }
        case 1:{
            
            do
			{   
                //Receive values from processes 0
				MPI_Recv(&disc, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Recv(&a_coeff, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Recv(&b_coeff, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

                //Calculate x1 and x2
                if(status.MPI_TAG != MSG_EXIT){
                    if(disc > 0){
                        x1 = (-b_coeff + sqrt(disc)) / (2 * a_coeff);
                        x2 = (-b_coeff - sqrt(disc)) / (2 * a_coeff);
                        
                        MPI_Send(&x1, 1, MPI_FLOAT, 2, MSG_REAL_SOLUTIONS, MPI_COMM_WORLD);
                        MPI_Send(&x2, 1, MPI_FLOAT, 2, MSG_REAL_SOLUTIONS, MPI_COMM_WORLD);
                    } else if (disc < 0){
                        x1 = (-b_coeff / (2 * a_coeff)) + (sqrt(abs(disc)) / (2 * a_coeff));
                        x2 = (-b_coeff / (2 * a_coeff)) - (sqrt(abs(disc)) / (2 * a_coeff));
                        
                        MPI_Send(&x1, 1, MPI_FLOAT, 2, MSG_COMPLEX_SOLUTIONS, MPI_COMM_WORLD);
                        MPI_Send(&x2, 1, MPI_FLOAT, 2, MSG_COMPLEX_SOLUTIONS, MPI_COMM_WORLD);
                    } else {
                        x1 = -b_coeff / (2 * a_coeff);
                        x2 = -b_coeff / (2 * a_coeff);
                        
                        MPI_Send(&x1, 1, MPI_FLOAT, 2, MSG_ONE_REAL_SOLUTION, MPI_COMM_WORLD);
                        MPI_Send(&x2, 1, MPI_FLOAT, 2, MSG_ONE_REAL_SOLUTION, MPI_COMM_WORLD);
                    }
                }
                else{
                    //Send exit message
                    MPI_Send(&x1, 1, MPI_FLOAT, 2, MSG_EXIT, MPI_COMM_WORLD);
                    MPI_Send(&x2, 1, MPI_FLOAT, 2, MSG_EXIT, MPI_COMM_WORLD);
                   
                }
				
			} while (status.MPI_TAG != MSG_EXIT);
            
            break;
        }
       
        case 2:{
            //Receieve element count
            MPI_Recv(&fileElementCount, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            //Write headers
            pOutFile = fopen("roots.txt","w+");
            fprintf(pOutFile,"%d \n",fileElementCount);
            fprintf(pOutFile,"x1\tx2\tx1_real\tx1_img\tx2_real\tx2_img\n");

            do {
                //Receive x1 and x2 from process 1
                MPI_Recv(&x1, 1, MPI_FLOAT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Recv(&x2, 1, MPI_FLOAT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            
                switch (status.MPI_TAG) {
                    case MSG_EXIT: {
                        break;
                    }
                    //Write to file
                    case MSG_REAL_SOLUTIONS: {
                        fprintf(pOutFile,"\t\t%.1f\t%.1f\t\n", x1, x2);
                        break;
                    }
                    
                    case MSG_COMPLEX_SOLUTIONS: {
                        fprintf(pOutFile,"\t\t\t%.1f\t%.1f\n", x1, x2);
                        break;
                    }
                    
                    case MSG_ONE_REAL_SOLUTION: {
                        fprintf(pOutFile,"%.1f\t%.1f\t\t\t\t\n", x1, x2);
                        break;
                    }
                }
              
            } while (status.MPI_TAG != MSG_EXIT);
            break;
        }
        default:{
			printf("Process %d unused.\n",my_rank);
			break;
		}
    }
    
    MPI_Finalize();
    return 0;
}
