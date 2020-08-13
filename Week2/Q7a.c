#include <stdio.h>
int main(){
     int c;
     int words=0;
     while ( (c = getchar()) != EOF ){

        if (c == ' '||c == '\n') words++;
        
     }

     printf("Number of words = %d\n", words);
    return(0);
}
