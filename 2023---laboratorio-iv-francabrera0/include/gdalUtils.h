#ifndef GDAL_UTILS_H
#define GDAL_UTILS_H

#include <stdint.h>

typedef struct Images {
    int     widht;
    int     height;
    uint8_t *redBand;
    uint8_t *greenBand;
    uint8_t *blueBand;
} Image;

typedef enum {
    RED = 1,
    GREEN = 2,
    BLUE = 3
} BANDS;

int getRasterBands(Image* inputImage, const char* fileName);
int saveImage(Image* inputImage);
void freeImg(Image* inputImage);

#endif /*GDAL_UTILS_H*/