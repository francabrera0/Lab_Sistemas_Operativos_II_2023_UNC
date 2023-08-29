/*
 * usb.h
 *
 *  Created on: Jul 29, 2023
 *      Author: francabrera
 */

#include "stdint.h"

#ifndef INC_USB_H_
#define INC_USB_H_

void vSendUsb(char *msgToSend);
uint8_t ucGetFilterParameter(uint8_t *msg);


#endif /* INC_USB_H_ */
