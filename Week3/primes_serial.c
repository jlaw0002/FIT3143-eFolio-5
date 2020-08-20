#include<stdio.h>
#include<math.h>
#include <stdbool.h>

bool isPrime(int n);

int main(){
    printf("Enter a number: ");

    // Read user input and store it in variable 'n'
    int n;
    scanf("%d", &n);

    // Create a file named "primes.txt"
    FILE *fp;
    fp = fopen("primes.txt", "w+");
    fprintf(fp, "Prime Number less than %d:\n",n);

    for(int i = 2; i <= n; i++){
        if(isPrime(i)){
            fprintf(fp, "%d\n", i);
        }
    }

    fclose(fp);
    printf("primes.txt created\n");
    return 0;
}

// Reference: https://www.geeksforgeeks.org/print-all-prime-numbers-less-than-or-equal-to-n/
bool isPrime(int n){
    // Edge cases
    if (n <= 1)
        return false;
    if (n <= 3)
        return true;

    // An optimisation to reduce the number of iterations in the 'for' loop
    if(n % 2 == 0 || n % 3 == 0)
        return false;

    for(int i = 5; i <= sqrt(n); i = i + 6){
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }

    return true;
}
