/*****************************************************************************
 *
 *	@file: main.c
 *	@brief: Main program body
 *			LabVII SOII - 2023. Temperature acquisition using FreeRTOS
 *
 *****************************************************************************/

/*Includes*/
#include "main.h"
#include "usb_device.h"

#include "usbd_cdc_if.h"
#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "usb.h"
#include "top.h"
#include "utils.h"
#include "filter.h"
#include "graph.h"
#include "dht11.h"

/*Config functions*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);

/*Timer handle*/
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

/*Task handlers*/
xTaskHandle xTemperatureSensorHandle;
xTaskHandle xLowPassFilterHandle;
xTaskHandle xPrintGraphHandle;
xTaskHandle xTopHandle;
xTaskHandle xCommandProcessHandle;

/*Queue handlers*/
xQueueHandle xTemperatureValuesQueueHandle;
xQueueHandle xFilteredTemepratureValuesQueueHandle;

/*Task functions*/
static void prvTemperatureSensor(void *params);
static void prvLowPassFilter(void *params);
static void prvPrintGraph(void *params);
static void prvTop(void *params);
static void prvCommandProcess(void *params);

/*Global Variables*/
uint8_t usbBuffer[32];
uint8_t ucFilterParameter = INITIAL_FILTER_PARAMETER;
float ulpTemperatureValues[MAX_NUM_OF_SAMPLES];
float dpFilteredTemperatureValues[MAX_NUM_OF_SAMPLES];


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {

	HAL_Init();

	SystemClock_Config();
	MX_TIM1_Init();
	HAL_TIM_Base_Start(&htim1);

	MX_GPIO_Init();
	MX_USB_DEVICE_Init();

	xTemperatureValuesQueueHandle = xQueueCreate(QUEUE_SIZE, sizeof(uint32_t));
	xFilteredTemepratureValuesQueueHandle = xQueueCreate(QUEUE_SIZE, sizeof(float));


	xTaskCreate(prvTemperatureSensor,
					"TemperatureSensor",
					TEMPERATURE_SENSOR_STACK_SIZE,
					NULL,
					TEMPERATURE_SENSOR_PRIORITY,
					&xTemperatureSensorHandle);

	xTaskCreate(prvLowPassFilter,
					"LowPassFilter",
					LOW_PASS_FILTER_STACK_SIZE,
					NULL,
					LOW_PASS_FILTER_PRIORITY,
					&xLowPassFilterHandle);

	xTaskCreate(prvPrintGraph,
					"PrintGraph",
					PRINT_GRAPH_STACK_SIZE,
					NULL,
					PRINT_GRAPH_PRIORITY,
					&xPrintGraphHandle);

	xTaskCreate(prvTop,
					"Top",
					TOP_STACK_SIZE,
					NULL,
					TOP_PRIORITY,
					&xTopHandle);

	xTaskCreate(prvCommandProcess,
					"CommandProcess",
					COMMAND_PROCESS_STACK_SIZE,
					NULL,
					COMMAND_PROCESS_PRIORITY,
					&xCommandProcessHandle);



	vTaskStartScheduler();

	while (1) {

	}
}


/**
  * @brief Temperature acquisition from DHT11 sensor.
  * 		This task takes the reading from the sensor and places it in a queue
  * 		to communicate with the prvLowPassFilter task.
  *
  * @param void *params not used
  *
  */
static void prvTemperatureSensor(void *params) {

	DHT11_init(&htim1);
	float temperature = 0;

	TickType_t xLastExecutionTime;
	xLastExecutionTime = xTaskGetTickCount();
	while(1) {

		if(!DHT11_getData(&temperature)) {
			xQueueSend(xTemperatureValuesQueueHandle, &temperature, portMAX_DELAY);
		}

		vTaskDelayUntil(&xLastExecutionTime, 100);
	}
}

/**
  * @brief This task receive a raw value from prvTemperatureSensor, places it at the beginning of an array
  * 		and obtain the average of the last "ucParameter" samples. Then, puts the filtered value in a queue
  * 		to communicate with prvPrintGraphTask.
  *
  *	@param void *params not used
  *
  */
static void prvLowPassFilter(void *params) {

	while(1) {

		float fValue = 0;
		xQueueReceive(xTemperatureValuesQueueHandle, &fValue, portMAX_DELAY);

		vPushRawValue(ulpTemperatureValues, fValue, MAX_NUM_OF_SAMPLES);

		uint8_t ucParameter = ucFilterParameter;

		float fResult = ulLowPassFilter(ulpTemperatureValues, ucParameter);

		xQueueSend(xFilteredTemepratureValuesQueueHandle, &fResult, portMAX_DELAY);

	}
}

/**
  * @brief This task receive a filtered value from prvLowPassFilter task, places it at the beginning of an array
  * 		and send this value by USB with an specific format to be graphed.
  *
  * @param void *params not used
  *
  */
static void prvPrintGraph(void *params) {

	while(1) {

		float fFilteredValue = 0;
		xQueueReceive(xFilteredTemepratureValuesQueueHandle, &fFilteredValue, portMAX_DELAY);

		vPushFilteredValue(dpFilteredTemperatureValues, fFilteredValue, MAX_NUM_OF_SAMPLES);

		char temperatureToSend[32];
		memset(temperatureToSend, '\0', strlen(temperatureToSend));
		sprintf(temperatureToSend, "TEMP:%f\r\n", fFilteredValue);

		vSendUsb(temperatureToSend);
	}
}

/**
  * @brief This task obtain the run time statistics of the tasks. This information is sent by USB.
  * 		For each task is sent: Name of the task, stack free, ticks in execution and CPU usage.
  *
  * @param void *params not used
  *
  */
static void prvTop(void *params) {

	while(1) {

		vGetRunTimeStats();

		vTaskDelay(1000);
	}
}

/**
  *	@brief This task is in charge of decoding incoming messages via USB. If the parameter is valid, then update the
  *			filter parameter value. This task is notified by the USB receiver ISR.
  *
  * @param void *params not used
  */
static void prvCommandProcess(void *params) {

	while(1) {
		xTaskNotifyWait(0x00, 0xffffffff, NULL, portMAX_DELAY);

		uint8_t pcCommand[32];
		memset(pcCommand, '\0', strlen((char*)pcCommand));
		memcpy(pcCommand, usbBuffer, strlen((char*)usbBuffer));
		uint8_t ucParameter;
		ucParameter = ucGetFilterParameter(pcCommand);

		if(ucParameter != 0) {
			ucFilterParameter = ucParameter;
		}

		vTaskDelay(10);
	}
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};


	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}


	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void) {

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 71;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 65535;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) {
		Error_Handler();
	}
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
void vConfigureTimerForRunTimeStats(void) {

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 65535;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_Base_Start(&htim2);
  __HAL_TIM_SET_COUNTER(&htim2, 0);

}

/**
  *
  *
  *
  */
 uint32_t ulGetTimerCounter(void) {
 	static uint16_t oldCounter;
 	static uint8_t ctrl;

 	uint16_t counter = __HAL_TIM_GET_COUNTER(&htim2);

 	if(oldCounter > counter) {
 		ctrl++;
 	}

 	oldCounter = counter;
 	return (uint32_t) ((65535*ctrl) + counter);

 }


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET);

	GPIO_InitStruct.Pin = DHT11_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {

	__disable_irq();
	while (1) {
	}
}
