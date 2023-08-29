/*
 * graph.c
 *
 *  Created on: Jul 29, 2023
 *      Author: francabrera
 */

#include "graph.h"

void vPushFilteredValue(float *fpArray, float fValue, uint8_t ucMaxArraySize) {
	for(uint8_t i = ucMaxArraySize-1; i>0; i--) {
			fpArray[i] = fpArray[i-1];
		}
		fpArray[0] = fValue;
}
