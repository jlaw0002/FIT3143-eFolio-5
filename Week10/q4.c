#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define MSG_EXIT 1
#define MSG_REAL_SOLUTIONS 2
#define MSG_COMPLEX_SOLUTIONS 3
#define MSG_ONE_REAL_SOLUTION 4

int main(int argc, char *argv[]){
    FILE *pInfile;
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
    
    pInfile = fopen("quad.txt","r");
    fscanf(pInfile, "%d", &fileElementCount);
    // Fix this line
    fscanf(pInfile, "%s\t%s\t%s", &a_coeff, &b_coeff, &c_coeff);
    
    switch (my_rank){
        case 0:{
            // Send the counter to the last process
			MPI_Send(&fileElementCount, 1, MPI_INT, (p - 1), 0, MPI_COMM_WORLD);

			// Read each element from the file
			while(counter < fileElementCount)
			{
				fscanf(pInfile, "%f\t&f\t&f", &a_coeff, &b_coeff, &c_coeff);
				disc = (b_coeff*b_coeff) - 4*a_coeff*c_coeff;
				MPI_Send(&disc, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
				MPI_Send(&a_coeff, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
				MPI_Send(&b_coeff, 1, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
				counter++;
			}
			
			fclose(pInfile);
			pInfile = NULL;
			
			MPI_Send(&disc, 1, MPI_FLOAT, 1, MSG_EXIT, MPI_COMM_WORLD);
			MPI_Send(&a_coeff, 1, MPI_FLOAT, 1, MSG_EXIT, MPI_COMM_WORLD);
			MPI_Send(&b_coeff, 1, MPI_FLOAT, 1, MSG_EXIT, MPI_COMM_WORLD);
			
            break;
        }
        case 1:{
            // WRITE PART (b) HERE
            do
			{ 
				MPI_Recv(&disc, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Recv(&a_coeff, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Recv(&b_coeff, 1, MPI_FLOAT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				
				if(disc > 0){
				    x1 = (-b_coeff + sqrt(disc)) / (2 * a_coeff);
				    x2 = (-b_coeff - sqrt(disc)) / (2 * a_coeff);
				    
				    MPI_Send(&x1, 1, MPI_DOUBLE, 2, MSG_REAL_SOLUTIONS, MPI_COMM_WORLD);
				    MPI_Send(&x2, 1, MPI_DOUBLE, 2, MSG_REAL_SOLUTIONS, MPI_COMM_WORLD);
				} else if (disc < 0){
				    x1 = (-b_coeff / (2 * a_coeff)) + (sqrt(abs(disc)) / (2 * a_coeff));
				    x2 = (-b_coeff / (2 * a_coeff)) - (sqrt(abs(disc)) / (2 * a_coeff));
				    
				    MPI_Send(&x1, 1, MPI_DOUBLE, 2, MSG_COMPLEX_SOLUTIONS, MPI_COMM_WORLD);
				    MPI_Send(&x2, 1, MPI_DOUBLE, 2, MSG_COMPLEX_SOLUTIONS, MPI_COMM_WORLD);
				} else {
				    x1 = -b_coeff / (2 * a_coeff);
				    x2 = -b_coeff / (2 * a_coeff);
				    
				    MPI_Send(&x1, 1, MPI_DOUBLE, 2, MSG_ONE_REAL_SOLUTION, MPI_COMM_WORLD);
				    MPI_Send(&x2, 1, MPI_DOUBLE, 2, MSG_ONE_REAL_SOLUTION, MPI_COMM_WORLD);
				}
			} while (status.MPI_TAG != MSG_EXIT);
            break;
        }
        case 2:{
            // WRITE PART (c) HERE
            do {
                MPI_Recv(&x1, 1, MPI_FLOAT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Recv(&x2, 1, MPI_FLOAT, 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            
                switch (status.MPI_TAG) {
                    case MSG_EXIT: {
                        break;
                    }
                    
                    case MSG_REAL_SOLUTIONS: {
                        printf("\t\t%f\t%f\t\n", x1, x2);
                        break;
                    }
                    
                    case MSG_COMPLEX_SOLUTIONS: {
                        printf("\t\t\t%f\t%f\n", x1, x2);
                        break;
                    }
                    
                    case MSG_ONE_REAL_SOLUTION: {
                        printf("%f%f\t\t\t\t\n", x1, x2);
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
