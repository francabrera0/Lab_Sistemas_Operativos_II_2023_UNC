/*
 * filter.h
 *
 *  Created on: Jul 29, 2023
 *      Author: francabrera
 */

#ifndef INC_FILTER_H_
#define INC_FILTER_H_

#include "stdint.h"

void vPushRawValue(float *fpArray, float fValue, uint8_t ucMaxArraySize);

float ulLowPassFilter(float *fpAarray, uint8_t ucFilterParameter);

#endif /* INC_FILTER_H_ */
