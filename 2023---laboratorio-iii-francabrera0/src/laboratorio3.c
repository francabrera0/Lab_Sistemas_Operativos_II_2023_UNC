#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int XDIM = 100;
int YDIM = 100;

/**
 * @brief Allocate the array
 * 
 * @return double** Allocate matrix
 */
double **alloc_matrix(void) {
    double **array;
    time_t t1; 
    srand ((unsigned) time (&t1));

    array = (double**)malloc((unsigned long)XDIM*sizeof(double *));
    if(array == NULL) {
        perror("Could not allocate row");
        exit(EXIT_FAILURE);
    }

    for(int i = 0 ; i < XDIM ; i++) {
        array[i] = (double*)malloc((unsigned long)YDIM*sizeof(double));
        if(array[i] == NULL) {
            perror("Could not allocate column");
            exit(EXIT_FAILURE);
        }
        for(int j=0; j<YDIM; j++)
            array[i][j] = (double)(1);
    }
    return array;
}

/**
 * @brief Computes the kernell
 * 
 * @param arr Matrix
 * @param kern Kernell
 */
void compute(double** arr, int kern[3][3]) {
    double accum = 0;
    printf("Processing...\n");
    for(int i = 0 ; i < XDIM ; i++) {
        //printf("processing row: %d \n", i);
        for(int j = 0 ; j < YDIM ; j++){
            if(i >= 1 && j >=1 && i < XDIM-1 && j <YDIM-1){
                accum = 0;
                for(int k = 0; k < 3; k++)
                    for(int l = 0; l < 3; l++)
                        accum += ((kern[l][k]*arr[i + (l-1)][j + (k-1)])*0.004 + 1);
            }
            arr[i][j] = accum;
        }    
    }
    printf("End\n");
}

/**
 * @brief Print the matrix
 * 
 * @param arr Matrix to print
 */
void print(double** arr) {
    FILE* file;
    file = fopen("output","w");
    setvbuf(file, NULL, _IOFBF, (size_t)(32*XDIM*YDIM));
    for(int i = 0 ; i < XDIM ; i++)
        for(int j = 0 ; j < YDIM ; j++)
            fprintf(file,"array[%d][%d] = %f\n", i, j, arr[i][j]);
    fclose(file);
}


/**
 * @brief Launcher 
 * 
 * @return int On success 0 is returned, on failure -1 is returned
 */
int main(void) {
    double **arr;
    int kern[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};

    arr = alloc_matrix();
    compute(arr, kern);
    print(arr);

    for(int i=0; i<XDIM; i++)
        free(arr[i]);
    free(arr);
    return 0;
}