/*
 * top.c
 *
 *  Created on: Jul 28, 2023
 *      Author: francabrera
 */

#include <stdio.h>

#include "top.h"
#include "usb.h"

#include "string.h"
#include "FreeRTOS.h"
#include "task.h"



void vGetRunTimeStats(void) {

	char pcStats[128];
	volatile UBaseType_t uxArraySize;
	volatile UBaseType_t x;
	unsigned long ulTotalRunTime;
	unsigned long ulStatsAsPercentage;

	uxArraySize = uxTaskGetNumberOfTasks();

	TaskStatus_t pxTaskStatusArray [uxArraySize];


	if(pxTaskStatusArray != NULL) {

		uxArraySize = uxTaskGetSystemState(pxTaskStatusArray,
										   uxArraySize,
										   &ulTotalRunTime);

		ulTotalRunTime /= 100UL;

		if(ulTotalRunTime > 0) {

			for(x = 0; x < uxArraySize; x++) {

	        	memset(pcStats, '\0', strlen(pcStats));

	            ulStatsAsPercentage = pxTaskStatusArray[x].ulRunTimeCounter/ulTotalRunTime;

	            sprintf(pcStats, "%s,%u,%u,%lu\r\n",
	                                 pxTaskStatusArray[x].pcTaskName,
									 (unsigned int)pxTaskStatusArray[x].usStackHighWaterMark,
	                                 (unsigned int)pxTaskStatusArray[x].ulRunTimeCounter,
	                                 ulStatsAsPercentage);

	            vSendUsb(pcStats);

	         }
		}
	}
}

