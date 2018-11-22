#include <stdio.h>
#include <stdlib.h>

int main () {
    int* n1;
    long double* n2;
    int* n3;
    long double* n4;
    int* n5;
    int* n6;
    long double* n7;

    n1 = (int*)malloc(sizeof(int));
    n2 = (long double*)malloc(sizeof(long double));
    n3 = (int*)malloc(sizeof(int));
    n4 = (long double*)malloc(sizeof(long double));
    n5 = (int*)malloc(sizeof(int));

    *n1 = 88000;    
    *n2 = 500;    
    *n3 = 1000;
    *n4 = 45000;
    *n5 = 2350;

    //print_myMalloc_information();
    free(n2);
    free(n3);
    free(n5);
    //print_myMalloc_information();

    n6 = (int*)malloc(sizeof(int));
    n7 = (long double*)malloc(sizeof(long double));

    *n6 = 2000;
    *n7 = 88800;
    //print_myMalloc_information();

}