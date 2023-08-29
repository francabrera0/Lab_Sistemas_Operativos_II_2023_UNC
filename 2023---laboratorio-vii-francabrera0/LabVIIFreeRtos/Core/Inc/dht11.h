/*
 * dht11.h
 *
 *  Created on: Aug 1, 2023
 *      Author: francabrera
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "main.h"

void DHT11_init(TIM_HandleTypeDef *timhandle);

uint8_t DHT11_getData(float * temperature);

#endif /* INC_DHT11_H_ */
