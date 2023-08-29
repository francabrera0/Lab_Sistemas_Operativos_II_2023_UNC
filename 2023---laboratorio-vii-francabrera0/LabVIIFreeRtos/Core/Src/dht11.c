/*
 * dht11.c
 *
 *  Created on: Aug 1, 2023
 *      Author: francabrera
 */

#include "dht11.h"


uint8_t RHI, RHD, TCI, TCD, SUM;
uint32_t pMillis, cMillis;
float tCelsius = 0;
float tFahrenheit = 0;
float RH = 0;
TIM_HandleTypeDef *htim;

static void microDelay(uint16_t delay);
static uint8_t DHT11_Start (void);
static uint8_t DHT11_Read (void);


void DHT11_init(TIM_HandleTypeDef *timhandle) {
	htim = timhandle;
}

uint8_t DHT11_getData(float * temperature) {

	if(DHT11_Start()) {
		RHI = DHT11_Read(); // Relative humidity integral
		RHD = DHT11_Read(); // Relative humidity decimal
		TCI = DHT11_Read(); // Celsius integral
		TCD = DHT11_Read(); // Celsius decimal
		SUM = DHT11_Read(); // Check sum
		if (RHI + RHD + TCI + TCD == SUM) {
		  *temperature = (float)TCI + (float)(TCD/10.0);
		  return 0;
		}
	  }
	return 1;
}

static void microDelay (uint16_t delay) {
  __HAL_TIM_SET_COUNTER(htim, 0);
  while (__HAL_TIM_GET_COUNTER(htim) < delay);
}

static uint8_t DHT11_Start (void) {
  uint8_t Response = 0;
  GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
  GPIO_InitStructPrivate.Pin = DHT11_Pin;
  GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStructPrivate); // set the pin as output
  HAL_GPIO_WritePin (DHT11_GPIO_Port, DHT11_Pin, 0);   // pull the pin low
  HAL_Delay(20);   // wait for 20ms
  HAL_GPIO_WritePin (DHT11_GPIO_Port, DHT11_Pin, 1);   // pull the pin high
  microDelay (30);   // wait for 30us
  GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStructPrivate); // set the pin as input
  microDelay (40);
  if (!(HAL_GPIO_ReadPin (DHT11_GPIO_Port, DHT11_Pin)))
  {
    microDelay (80);
    if ((HAL_GPIO_ReadPin (DHT11_GPIO_Port, DHT11_Pin))) Response = 1;
  }
  pMillis = HAL_GetTick();
  cMillis = HAL_GetTick();
  while ((HAL_GPIO_ReadPin (DHT11_GPIO_Port, DHT11_Pin)) && pMillis + 2 > cMillis)
  {
    cMillis = HAL_GetTick();
  }
  return Response;
}

static uint8_t DHT11_Read (void) {
  uint8_t a,b;
  for (a=0;a<8;a++)
  {
    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while (!(HAL_GPIO_ReadPin (DHT11_GPIO_Port, DHT11_Pin)) && pMillis + 2 > cMillis)
    {  // wait for the pin to go high
      cMillis = HAL_GetTick();
    }
    microDelay (40);   // wait for 40 us
    if (!(HAL_GPIO_ReadPin (DHT11_GPIO_Port, DHT11_Pin)))   // if the pin is low
      b&= ~(1<<(7-a));
    else
      b|= (1<<(7-a));
    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while ((HAL_GPIO_ReadPin (DHT11_GPIO_Port, DHT11_Pin)) && pMillis + 2 > cMillis)
    {  // wait for the pin to go low
      cMillis = HAL_GetTick();
    }
  }
  return b;
}
