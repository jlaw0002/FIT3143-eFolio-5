#include <stdio.h>
int main() {
    int x= 10;
    int y = 5;

    //(Equal statements)
    if (x%2==0){
        printf("X is even\n");
    }
    else if(x%2==1){
        printf("X is odd\n");
    }
    //And, or and not statements
    if (x%2==0 && y%2==0){
        printf("both x and y are even \n");
    }
    else if (x%2==0 || y%2==0){
        printf("atleast one of x and y are even \n");
    }
    else if (!x%2==0 && !y%2==0){
        printf("both x and y are not even \n");
    }
    //Less than and greter than
    if (x<y){
        printf("X less then y\n");
    }
    
    else if (y<x){
        printf("y less than x\n");
        
    }
    else{
        printf("y is the same as x\n");
    }
   //Switch Statements
    switch(x){
        case 5:
            printf("x is 5\n");
            break;
            
        case 10:
            printf("x is 10\n");
            break;
            
        default:
            printf("x is not 5 or 10\n");
            break;
    }
}
