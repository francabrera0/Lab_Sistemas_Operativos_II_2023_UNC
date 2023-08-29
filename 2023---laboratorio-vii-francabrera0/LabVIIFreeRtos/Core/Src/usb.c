/*
 * usb.c
 *
 *  Created on: Jul 29, 2023
 *      Author: francabrera
 */

#include "usb.h"
#include "usbd_cdc_if.h"

#include "string.h"
#include "stdio.h"
#include "utils.h"


void vSendUsb(char *msgToSend) {

	CDC_Transmit_FS((uint8_t*) msgToSend, strlen(msgToSend));

}

uint8_t ucGetFilterParameter(uint8_t *msg) {

	uint8_t ucValue;
	char *pcPtr = strstr((char*)msg, "\"FILTER_PARAM\" : ");

	if(pcPtr != NULL) {
		pcPtr += strlen("\"FILTER_PARAM\" : ");

		ucValue = (uint8_t) atoi(pcPtr);

		if(ucValue > 0 && ucValue <= MAX_NUM_OF_SAMPLES) {
			char cpAck[16];
			sprintf(cpAck, "FILTER: %d\r\n", ucValue);
			vSendUsb(cpAck);
			return ucValue;
		}
	}

	return (uint8_t) 0;


}
