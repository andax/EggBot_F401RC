/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "usbd_cdc_if.h"
#include "File_Handling.h"
#include "parser.h"
#include "stepper.h"
#include "servo.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern char SDPath[4];   /* SD logical drive path */
extern FATFS SDFatFS;    /* File system object for SD logical drive */
FRESULT res;

extern int32_t Motor1_CurrentPos;
extern int32_t Motor2_CurrentPos;
extern bool penState;
bool userButton = false;
bool ledRed = false;
bool ledGreen = false;
bool ledBlue = false;


osPoolDef(commandPool, 8, command_t);
osPoolId  commandPool_Id;

osPoolDef(motionPool, 32, motion_t);
osPoolId  motionPool_Id;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId dispatchTaskHandle;
osMessageQId cmdQueueHandle;
uint8_t cmdQueueBuffer[ 8 * sizeof( uint32_t ) ];
osStaticMessageQDef_t cmdQueueControlBlock;
osMessageQId motionQueueHandle;
uint8_t motionQueueBuffer[ 16 * sizeof( uint32_t ) ];
osStaticMessageQDef_t motionQueueControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartDispatchTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of cmdQueue */
  osMessageQStaticDef(cmdQueue, 8, uint32_t, cmdQueueBuffer, &cmdQueueControlBlock);
  cmdQueueHandle = osMessageCreate(osMessageQ(cmdQueue), NULL);

  /* definition and creation of motionQueue */
  osMessageQStaticDef(motionQueue, 16, uint32_t, motionQueueBuffer, &motionQueueControlBlock);
  motionQueueHandle = osMessageCreate(osMessageQ(motionQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of dispatchTask */
  osThreadDef(dispatchTask, StartDispatchTask, osPriorityIdle, 0, 128);
  dispatchTaskHandle = osThreadCreate(osThread(dispatchTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
#if 0
  res = f_mount(&SDFatFS, (TCHAR const*)SDPath, 1);
  if (res != 0)
  {
	  char msg[] = "Error Mounting disk!\r\n";
	  CDC_Transmit_FS((uint8_t*) msg, (uint16_t) strlen(msg));
  }
  else
  {
	  char msg[] = "Mounting disk ok!\r\n";
	  CDC_Transmit_FS((uint8_t*) msg, (uint16_t) strlen(msg));
  }

  Scan_SD("0:/");
#endif
  /* Infinite loop */
  for(;;)
  {
	  printf("Red\r\n");
	  ITM_SendChar('R');
	  ledRed = true;
	  osDelay(500);
	  ledRed = false;
	  osDelay(500);
	  printf("Green\r\n");
	  ITM_SendChar('G');
	  ledGreen = true;
	  osDelay(500);
	  ledGreen = false;
	  osDelay(500);
	  printf("Blue\r\n");
	  ITM_SendChar('B');
	  ledBlue = true;
	  osDelay(500);
	  ledBlue = false;
	  osDelay(500);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartDispatchTask */
/**
* @brief Function implementing the dispatchTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDispatchTask */
void StartDispatchTask(void const * argument)
{
  /* USER CODE BEGIN StartDispatchTask */
  /* Initialize cmd pool */
  commandPool_Id = osPoolCreate(osPool(commandPool));  // create memory pool
  /* Initialize motion pool */
  motionPool_Id = osPoolCreate(osPool(motionPool));  // create memory pool
  char reply_msg[50];
  /* Infinite loop */
  for(;;)
  {
		osEvent event;
		command_t* pCommand;
		event = osMessageGet(cmdQueueHandle, osWaitForever);
		if (event.status == osEventMessage)
		{
			pCommand = event.value.p;

			switch (pCommand->cmd)
			{
			case CMD_EM: // Enable Motors - EM,Enable1[,Enable2]<CR>
				Stepper_MotorEnable(pCommand->value1, pCommand->value2);
				TransmitMsg("OK\r\n");
				break;

			case CMD_QB:
				if (userButton)
				{
					userButton = false;
					TransmitMsg("1\r\nOK\r\n");
				}
				else
					TransmitMsg("0\r\nOK\r\n");
				break;

			case CMD_QP:
				if (penState)
					TransmitMsg("1\r\nOK\r\n");
				else
					TransmitMsg("0\r\nOK\r\n");
				break;

			case CMD_QS:
				sprintf(reply_msg, "%ld,%ld\r\nOK\r\n", Motor1_CurrentPos, Motor2_CurrentPos);
				TransmitMsg(reply_msg);
				break;

			case CMD_R:
				TransmitMsg("Not Implemented Yet\r\n");
				break;

			case CMD_SC:
				switch(pCommand->value1)
				{
				case 4:
					SetPenUpValue(pCommand->value2);
					break;
				case 5:
					SetPenDownValue(pCommand->value2);
					break;
				case 100:
					SetPWMTest(pCommand->value2);
					break;
				}
				TransmitMsg("OK\r\n");
				break;

			case CMD_SE:
				TransmitMsg("Not Implemented Yet\r\n");
				break;

			case CMD_SM: // Stepper Move - SM,duration,axis1,axis2<CR>
				Stepper_Move(pCommand->value1, pCommand->value2, pCommand->value3);
				TransmitMsg("OK\r\n");
				break;

			case CMD_SP: // Set Pen - SP,value<CR>
				while (Stepper_Active())
					osDelay(5);
				SetPen(pCommand->value1);
				TransmitMsg("OK\r\n");
				break;

			case CMD_TP: // Toggle Pen - TP<CR>
				while (Stepper_Active())
					osDelay(5);
				TogglePen();
				TransmitMsg("OK\r\n");
				break;

			case CMD_V:
				TransmitMsg("EBBvTBD EggBot FW v0.1 (c) 2021 Androscientia\r\n");
				break;

			default:
				TransmitMsg("ERROR\r\n");
				break;
			}

			osPoolFree(commandPool_Id, pCommand); // free memory allocated for message
		}
  }
  /* USER CODE END StartDispatchTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == User_Button_Pin)
	{
		userButton = true;
	}
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
