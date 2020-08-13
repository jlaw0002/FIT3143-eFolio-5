#include <stdio.h>
int main() {
    

    
    int total=0;
    printf("Adding 10 to the variable total=%d through 3 different type of loops: \n",total);
    
    //For Loop 
    for (int x=0; x<10;x++) {
        total+=1;
    }
    printf("Total after for loop: %d\n",total);
     
 
    //While Loop
    int x = 0;
    while (x<10) {
        total+=1;
        x++;
    }
    printf("Total after while loop: %d\n",total);
    
    //Infinite Loop
  
    for (;;) {
        total+=1;
        if (total==30){
            break;
        }
    }
    printf("Total after infinite loop: %d\n",total);
    
    //Return successful code
    return (0);
}
