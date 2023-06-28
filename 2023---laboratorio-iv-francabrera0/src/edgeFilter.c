#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "edgeFilter.h"

int kernel[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};

/**
 * @brief Applies the edge filter to the band passed as a parameter
 * 
 * @param band Band to filter
 * @param widht Band widht
 * @param height Band height
 * @return uint8_t* Filtered band
 */
uint8_t* edgeFilter(uint8_t* band, int widht, int height, int threads) {
    double accum = 0;
    double data = 0;
    uint8_t* outBand = (uint8_t*)malloc(sizeof(uint8_t)*(size_t)widht*(size_t)height);

    omp_set_num_threads(threads);
    double start = omp_get_wtime();

    #pragma omp parallel for collapse (2)
    for(int i = 0 ; i < widht ; i++) {
        for(int j = 0 ; j < height ; j++) {
            if(i >= 1 && j >=1 && i < widht-1 && j < height-1) {
                accum = 0;
                for(int k = 0; k < 3; k++) {
                    for(int l = 0; l < 3; l++) {
                        int x = i + (k-1);
                        int y = j + (l-1);
                        data = band[(widht*x)+y];
                        accum += (kernel[k][l]*data);
                    }
                }
            }
            accum = (accum<0) ? 0:accum;
            accum = (accum>255) ? 255:accum;
            outBand[(widht*i)+j] = (uint8_t) accum;   
        }
    }
    double end = omp_get_wtime();
    printf("Filter Took %f seconds\n", end-start);
    return outBand;
}