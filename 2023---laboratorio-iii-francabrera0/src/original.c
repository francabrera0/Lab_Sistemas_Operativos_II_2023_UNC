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
    int i, j, k; 
    double **array;
    array = (double**)malloc((unsigned long)XDIM*sizeof(double *));
    if(array == NULL) {
        perror("Could not allocate row");
        exit(EXIT_FAILURE);
    }

    for(i = 0 ; i < XDIM ; i++) {
        array[i] = (double*)malloc((unsigned long)YDIM*sizeof(double));
        if(array[i] == NULL) {
            perror("Could not allocate column");
            exit(EXIT_FAILURE);
        }
    }
  
    for(j=0; j<XDIM; j++)
        for(k=0; k<YDIM; k++)
            memset(&array[k][j], j, sizeof(double));
    return array;
}

/**
 * @brief Fill the matrix with random values
 * 
 * @param arr Matrix to fill
 */
void fill(double** arr) {
    int i, j;
    time_t t1; 
    srand ((unsigned) time (&t1));
    for(i = 0 ; i < XDIM ; i++)
        for(j = 0 ; j < YDIM ; j++)
            arr[i][j] = (double)(rand() % 100);
}

/**
 * @brief Computes the kernell
 * 
 * @param arr Matrix
 * @param kern Kernell
 */
void compute(double** arr, int kern[3][3]) {
    double tmp_sum[9];
    double dato, accum;
    int i, j, k, l;
    for(i = 0 ; i < XDIM ; i++)
        for(j = 0 ; j < YDIM ; j++){
            printf("processing: %d - %d \n", i, j);
            if(i >= 1 && j >=1 && i < XDIM-1 && j <YDIM-1){
                for(k = 0; k < 3; k++)
                    for(l = 0; l < 3; l++){
                        int x = i + (l-1);
                        int y = j + (k-1);
                        dato = arr[x][y];
                        tmp_sum[l*3+k] = 2*(2*kern[l][k]*dato)/1000 + 1;
                    }

                accum = 0;
                for(k = 0; k < 3; k++)
                    for(l = 0; l < 3; l++)
                        accum = accum + tmp_sum[k*3+l];
            }
            arr[i][j] = accum;
        }    
}

/**
 * @brief Print the matrix
 * 
 * @param arr Matrix to print
 */
void print(double** arr) {
    int i, j;
    for(i = 0 ; i < XDIM ; i++)
        for(j = 0 ; j < YDIM ; j++)
            printf("array[%d][%d] = %f\n", i, j, arr[i][j]);
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
    fill(arr);
    compute(arr, kern);
    print(arr);

    return 0;
}