#include <stdlib.h>
#include <stdio.h>
#include <gdal/gdal.h>
#include <gdal/cpl_conv.h>
#include <gdal/cpl_string.h>
#include "gdalUtils.h"

/**
 * @brief Gets an array with the values of the corresponding band
 * 
 * @param dataset Dataset wit input image information
 * @param band Color band. Value must be RED, GREEN or BLUE
 * @param widht Image widht
 * @param height Image height
 * @return uint8_t* On success the corresponding array is returned. On failure NULL is returned.
 */
static uint8_t* getArrayBand(GDALDatasetH dataset, BANDS band, int* widht, int* height) {
    GDALRasterBandH hBand;
    hBand = GDALGetRasterBand(dataset, band);

    uint8_t *array;
    *widht = GDALGetRasterBandXSize(hBand);
    *height = GDALGetRasterBandYSize(hBand);
    array = (uint8_t *) CPLMalloc(sizeof(uint8_t)*(size_t)*widht*(size_t)*height);
    CPLErr erno = GDALRasterIO(hBand, GF_Read, 0, 0, *widht, *height,
                array, *widht, *height, GDT_Byte,
                0, 0);
    if(erno != CE_None) {
        perror("Failed Raster IO");
        return NULL;
    }
    return array;
}

/**
 * @brief Gets the raster bands from an image
 * 
 * @param inputImage Image struct to save raster bands
 * @param fileName path to image
 * @return int On success EXIT_SUCCESS is returned. On failure EXIT_FAILURE is returned. 
 */
int getRasterBands(Image* inputImage, const char* fileName) {
    GDALDatasetH  hDataset;
    GDALAllRegister();
    const GDALAccess eAccess = GA_ReadOnly;
    hDataset = GDALOpen(fileName, eAccess);
    if(hDataset == NULL) {
        perror("Open failure");
        return EXIT_FAILURE;
    }

    int xDimBands[3] = {0, 0, 0};
    int yDimBands[3] = {0, 0, 0};

    inputImage->redBand = getArrayBand(hDataset, RED, xDimBands, yDimBands);
    if(inputImage->redBand == NULL) {
        perror("Failed red band");
        return EXIT_FAILURE;
    }
    inputImage->greenBand = getArrayBand(hDataset, GREEN, xDimBands+1, yDimBands+1);
    if(inputImage->greenBand == NULL) {
        perror("Failed green band");
        return EXIT_FAILURE;
    }
    inputImage->blueBand = getArrayBand(hDataset, BLUE, xDimBands+2, yDimBands+2);
    if(inputImage->blueBand == NULL) {
        perror("Failed blue band");
        return EXIT_FAILURE;
    }

    for(int i=1; i<3; i++) {
        if(xDimBands[0] != xDimBands[i] || yDimBands[0] != yDimBands[i]) {
            perror("Invalid size of array");
            return EXIT_FAILURE;
        }
    }

    inputImage->widht = xDimBands[0];
    inputImage->height = yDimBands[0];
    return EXIT_SUCCESS;
}


/**
 * @brief Save the band values into a dataset
 * 
 * @param dataset Data set to save raster band
 * @param array Array with the values of raster band
 * @param band Color band. Value must be RED, GREEN or BLUE
 * @param widht Image widht
 * @param height Image height
 * @return int On success EXIT_SUCCESS is returned. On failure EXIT_FAILURE is returned. 
 */
static int saveBand(GDALDatasetH dataset, uint8_t* array, BANDS band, int widht, int height) {
    GDALRasterBandH hBandOut = GDALGetRasterBand(dataset, band);
    CPLErr erno = GDALRasterIO(hBandOut, GF_Write, 0, 0, widht, height, array, widht, height, GDT_Byte, 0, 0);
    if(erno != CE_None){
        perror("Saving band");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 * @brief Creates a new image from the structure passed as a parameter
 * 
 * @param inputImage Image struct
 * @return int On success EXIT_SUCCESS is returned. On failure EXIT_FAILURE is returned. 
 */
int saveImage(Image* inputImage) {
    const char *pszFormat = "GTiff";
    GDALDriverH hDriver = GDALGetDriverByName(pszFormat);
    if(hDriver == NULL) {
        return EXIT_FAILURE;
    }

    char **papszOptions = NULL;
    char *fileName = "out.tif";
    GDALDatasetH hDatasetOut = GDALCreate(hDriver, fileName, inputImage->widht, inputImage->height , 3, GDT_Byte, papszOptions);

    if(saveBand(hDatasetOut, inputImage->redBand, RED, inputImage->widht, inputImage->height)) {
        return EXIT_FAILURE;
    }
    if(saveBand(hDatasetOut, inputImage->greenBand, GREEN, inputImage->widht, inputImage->height)) {
        return EXIT_FAILURE;
    }
    if(saveBand(hDatasetOut, inputImage->blueBand, BLUE, inputImage->widht, inputImage->height)) {
        return EXIT_FAILURE;
    }

    GDALClose(hDatasetOut);
    return EXIT_SUCCESS;
}
/**
 * @brief Free up alocated memory
 * 
 * @param inputImage Image struct 
 */
void freeImg(Image* inputImage) {
    CPLFree(inputImage->redBand);
    CPLFree(inputImage->greenBand);
    CPLFree(inputImage->blueBand);
}
