/*
 * utils.h
 *
 *  Created on: Jul 29, 2023
 *      Author: francabrera
 */

#ifndef INC_UTILS_H_
#define INC_UTILS_H_

#include "FreeRTOSConfig.h"

#define MAX_NUM_OF_SAMPLES			20
#define QUEUE_SIZE					5

#define TEMPERATURE_SENSOR_PRIORITY  	configMAX_PRIORITIES-1
#define LOW_PASS_FILTER_PRIORITY		configMAX_PRIORITIES-2
#define PRINT_GRAPH_PRIORITY			configMAX_PRIORITIES-3
#define TOP_PRIORITY					configMAX_PRIORITIES-3
#define COMMAND_PROCESS_PRIORITY		configMAX_PRIORITIES-2

#define TEMPERATURE_SENSOR_STACK_SIZE	64
#define LOW_PASS_FILTER_STACK_SIZE		128
#define PRINT_GRAPH_STACK_SIZE			256
#define TOP_STACK_SIZE					256
#define COMMAND_PROCESS_STACK_SIZE		256


#endif /* INC_UTILS_H_ */
