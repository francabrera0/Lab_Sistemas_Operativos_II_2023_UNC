#include <stdio.h>
#include <stdlib.h>
#include "gdalUtils.h"
#include "edgeFilter.h"

/**
 * @brief Launcher
 * 
 * @param argc number of arguments
 * @param argv .tif file, number of threads
 * @return int On success EXIT_SUCCESS is returned. On failure EXIT_FAILURE is returned
 */
int main(int argc, char const *argv[]) {
    if (argc != 3) {
        perror("Please, enter the path to .tif file and the number of threads");
        exit(EXIT_FAILURE);
    }

    const char *fileName = argv[1];
    
    int threadNumber = atoi(argv[2]);
    if(threadNumber == 0) {
        perror("Invalid number of threads");
        exit(EXIT_FAILURE);
    }

    Image inputImage = {0, 0, 0, 0, 0};
    if(getRasterBands(&inputImage, fileName)) {
        freeImg(&inputImage);
        perror("Could not get raster bands");
        return EXIT_FAILURE;
    }
    
    printf("Thread number: %d\n", threadNumber);
    inputImage.redBand = edgeFilter(inputImage.redBand, inputImage.widht, inputImage.height, threadNumber);
    inputImage.greenBand = edgeFilter(inputImage.greenBand, inputImage.widht, inputImage.height, threadNumber);
    inputImage.blueBand = edgeFilter(inputImage.blueBand, inputImage.widht, inputImage.height, threadNumber);

    if(saveImage(&inputImage)) {
        freeImg(&inputImage);
        perror("Could not save image");
        return EXIT_FAILURE;
    }

    freeImg(&inputImage);
    return EXIT_SUCCESS;
}
