/*
 * filter.c
 *
 *  Created on: Jul 29, 2023
 *      Author: francabrera
 */


#include "filter.h"

void vPushRawValue(float *fpArray, float fValue, uint8_t ucMaxArraySize) {

	for(uint8_t i = ucMaxArraySize-1; i>0; i--) {
		fpArray[i] = fpArray[i-1];
	}
	fpArray[0] = fValue;
}

float ulLowPassFilter(float *fpArray, uint8_t ucFilterParameter) {
	double acum = 0;
	for(uint8_t i=0; i<ucFilterParameter; i++) {
		acum += fpArray[i];
	}
	return (float) acum/ucFilterParameter;
}
