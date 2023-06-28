#ifndef EDGE_FILTER_H
#define EDGE_FILTER_H
#include "gdalUtils.h"

uint8_t* edgeFilter(uint8_t* band, int widht, int height, int threads);

#endif /*EDGE_FILTER_H*/