/*
 * stepper.c
 *
 *  Created on: Mar 11, 2021
 *      Author: Andreas
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "stepper.h"
#include <math.h>
#include <stdbool.h>
#include "cmsis_os.h"


/* Private define ------------------------------------------------------------*/

/* Size of DMA step/dir buffer */
#define MOTION_DMA_BUFLEN (512)

#define MOTOR_1_DIR_SET 	0x00001000
#define MOTOR_1_DIR_RESET 	0x10000000
#define MOTOR_1_STEP_SET	0x00002000
#define MOTOR_1_STEP_RESET	0x20000000
#define MOTOR_2_DIR_SET 	0x00004000
#define MOTOR_2_DIR_RESET 	0x40000000
#define MOTOR_2_STEP_SET	0x00008000
#define MOTOR_2_STEP_RESET	0x80000000

#define MIN_STEP_DELTA 		0.000025
#define DMA_TIME_DELTA      0.00001

/* Private variables ---------------------------------------------------------*/

extern bool ledRed;
extern bool ledGreen;
extern bool ledBlue;

extern osMessageQId motionQueueHandle;
extern osPoolId  motionPool_Id;

/* Buffer for usage of DMA GPIO step/dir generation */
uint32_t motion_dmabuf[MOTION_DMA_BUFLEN] __attribute__((aligned));

int32_t Motor1_TargetPos = 0;
int32_t Motor2_TargetPos = 0;
int32_t Motor1_CurrentPos = 0;
int32_t Motor2_CurrentPos = 0;
uint32_t Motor_Dir_Pattern = 0;
int32_t Motor1_DeltaStep;
int32_t Motor2_DeltaStep;
uint8_t Motor_Motion_Active = 0;

float Motor1_Step_Time = 0.0;
float Motor2_Step_Time = 0.0;
float Motor1_Step_Delta = 0.0;
float Motor2_Step_Delta = 0.0;

/* User code -----------------------------------------------------------------*/

uint8_t Stepper_Active()
{
	return Motor_Motion_Active;
}

void Stepper_Move(int32_t duration, int32_t axis1,  int32_t axis2)
{
	motion_t* pMotion;
	pMotion = osPoolAlloc(motionPool_Id); // Allocate memory for the motion

	pMotion->duration = duration;
	pMotion->axis1    = axis1;
	pMotion->axis2    = axis2;

	osMessagePut(motionQueueHandle, (uint32_t)pMotion, osWaitForever);  // Send motion to queue
}

void FillStepperDMABuffer(uint16_t start_ix, uint16_t stop_ix)
{
	uint32_t bit_pattern;
	float curr_time = 0.0;
	for (uint16_t i = start_ix; i < stop_ix; i++)
	{
		if ((Motor1_TargetPos == Motor1_CurrentPos) && (Motor2_TargetPos == Motor2_CurrentPos))
		{
			// At target. Let's check if there is new requested movements in the queue
			osEvent event;
			motion_t* pMotion;
			event = osMessageGet(motionQueueHandle, 0);
			if (event.status == osEventMessage)
			{
				pMotion = event.value.p;
				Motor1_TargetPos += pMotion->axis1;
				Motor2_TargetPos += pMotion->axis2;

				float duration = pMotion->duration/1000.0f;
				Motor1_Step_Delta = fabs(duration/(Motor1_TargetPos - Motor1_CurrentPos));
				if (Motor1_Step_Delta < MIN_STEP_DELTA)
					Motor1_Step_Delta = MIN_STEP_DELTA;
				Motor2_Step_Delta = fabs(duration/(Motor2_TargetPos - Motor2_CurrentPos));
				if (Motor2_Step_Delta < MIN_STEP_DELTA)
					Motor2_Step_Delta = MIN_STEP_DELTA;

				Motor_Dir_Pattern = 0;
				if (Motor1_TargetPos > Motor1_CurrentPos)
				{
					Motor_Dir_Pattern |= MOTOR_1_DIR_SET;
					Motor1_DeltaStep = 1;
				}
				else
				{
					Motor_Dir_Pattern |= MOTOR_1_DIR_RESET;
					Motor1_DeltaStep = -1;
				}
				if (Motor2_TargetPos > Motor2_CurrentPos)
				{
					Motor_Dir_Pattern |= MOTOR_2_DIR_SET;
					Motor2_DeltaStep = 1;
				}
				else
				{
					Motor_Dir_Pattern |= MOTOR_2_DIR_RESET;
					Motor2_DeltaStep = -1;
				}

				osPoolFree(motionPool_Id, pMotion); // free memory allocated for message
				Motor_Motion_Active = 1;

			}
			else
			{
				Motor_Motion_Active = 0;
			}
			bit_pattern = Motor_Dir_Pattern;
			bit_pattern |= MOTOR_1_STEP_RESET;
			bit_pattern |= MOTOR_2_STEP_RESET;

			motion_dmabuf[i] = bit_pattern;
		}
		else
		{
			bit_pattern = Motor_Dir_Pattern;

			if (Motor1_TargetPos != Motor1_CurrentPos)
			{
				if (Motor1_Step_Time <= curr_time)
				{
					bit_pattern |= MOTOR_1_STEP_SET;
					Motor1_CurrentPos += Motor1_DeltaStep;
					Motor1_Step_Time  += Motor1_Step_Delta;
				}
				else
				{
					bit_pattern |= MOTOR_1_STEP_RESET;
				}
			}
			else
			{
				bit_pattern |= MOTOR_1_STEP_RESET;
			}

			if (Motor2_TargetPos != Motor2_CurrentPos)
			{
				if (Motor2_Step_Time <= curr_time)
				{
					bit_pattern |= MOTOR_2_STEP_SET;
					Motor2_CurrentPos += Motor2_DeltaStep;
					Motor2_Step_Time  += Motor2_Step_Delta;
				}
				else
				{
					bit_pattern |= MOTOR_2_STEP_RESET;
				}
			}
			else
			{
				bit_pattern |= MOTOR_2_STEP_RESET;
			}

			motion_dmabuf[i] = bit_pattern;

			curr_time += DMA_TIME_DELTA;
		}
	}
	if (Motor1_Step_Time >= curr_time-DMA_TIME_DELTA)
	{
		Motor1_Step_Time -= curr_time;
	}
	if (Motor2_Step_Time >= curr_time-DMA_TIME_DELTA)
	{
		Motor2_Step_Time -= curr_time;
	}
}

/*!
* @brief Callback function for DMA half complete transfer
*/
void DMA_Xfer_Halfcplt_Callback(DMA_HandleTypeDef *_hdma)
{
	HAL_GPIO_WritePin(GPIO_0_GPIO_Port, GPIO_0_Pin, GPIO_PIN_SET);

	if (ledRed)
		HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
	if (ledGreen)
		HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
	if (ledBlue)
		HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET);

	/* First half of buffer finished. Fill this part with new data */
	FillStepperDMABuffer(0, MOTION_DMA_BUFLEN / 2);

	HAL_GPIO_WritePin(GPIO_0_GPIO_Port, GPIO_0_Pin, GPIO_PIN_RESET);
}

/*!
* @brief Callback function for DMA full complete transfer
*/
void DMA_Xfer_Cplt_Callback(DMA_HandleTypeDef *_hdma)
{
	HAL_GPIO_WritePin(GPIO_0_GPIO_Port, GPIO_0_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET);

	/* Second half of buffer finished. Fill this part with new data */
	FillStepperDMABuffer(MOTION_DMA_BUFLEN / 2, MOTION_DMA_BUFLEN);

	HAL_GPIO_WritePin(GPIO_0_GPIO_Port, GPIO_0_Pin, GPIO_PIN_RESET);
}

/*!
* @brief Write Registers in Motor Drivers
*/
HAL_StatusTypeDef Stepper_Write_Register(uint32_t stepper_1, uint32_t stepper_2)
{
	uint8_t wbuff[5];
	uint8_t rbuff[5];

	wbuff[0] = stepper_2 >> 12;
	wbuff[1] = stepper_2 >> 4;
	wbuff[2] = stepper_2 << 4 | stepper_1 >> 16;
	wbuff[3] = stepper_1 >> 8;
	wbuff[4] = stepper_1;

	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET);
	HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(&hspi1, wbuff, rbuff, 5, 1000);
	HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET);
	return ret;
}

/*!
* @brief Enable Motor Drivers
*/
void Stepper_MotorEnable(int32_t enable1, int32_t enable2)
{
	/* Enable Motor Drivers */
	if (enable1)
	{
		HAL_GPIO_WritePin(Motor_1_En_GPIO_Port, Motor_1_En_Pin, GPIO_PIN_RESET); // Enable Motor 1
	}
	else
	{
		HAL_GPIO_WritePin(Motor_1_En_GPIO_Port, Motor_1_En_Pin, GPIO_PIN_SET);   // Disable Motor 1
	}
	if (enable2)
	{
		HAL_GPIO_WritePin(Motor_2_En_GPIO_Port, Motor_2_En_Pin, GPIO_PIN_RESET); // Enable Motor 2
	}
	else
	{
		HAL_GPIO_WritePin(Motor_2_En_GPIO_Port, Motor_2_En_Pin, GPIO_PIN_SET);   // Disable Motor 2
	}
}


/*!
* @brief Initialize Stepper Motor Drivers
*/
void Stepper_Init()
{
	/* Setup Motor Driver Registers via SPI */
	Stepper_Write_Register(0x901B4, 0x901B4); /* CHOPCONF */ // Hysteresis mode
	Stepper_Write_Register(0xD1F0F, 0xD1F0F); /* SGCSCONF */ // Current setting: 0xD001F (max current)
	Stepper_Write_Register(0xE0010, 0xE0010); /* DRVCONF  */ // Low driver strength, stallGuard2 read, SDOFF=0
	Stepper_Write_Register(0x00004, 0x00004); /* DRVCTRL  */ // 16 microstep setting
	Stepper_Write_Register(0xA8200, 0xA8200); /* SMARTEN  */ // Enable coolStep with minimum current 1/4 CS

	/* Clear motion dma buffer to ensure no unwanted movements */
	for (int i=0; i < MOTION_DMA_BUFLEN;i++)
	{
		  motion_dmabuf[i] = 0;
	}

	/* Setup DMA callback functions */
	HAL_DMA_RegisterCallback(&hdma_tim1_up, HAL_DMA_XFER_HALFCPLT_CB_ID, DMA_Xfer_Halfcplt_Callback);
	HAL_DMA_RegisterCallback(&hdma_tim1_up, HAL_DMA_XFER_CPLT_CB_ID, DMA_Xfer_Cplt_Callback);

	/* Enable the TIM Update DMA request */
	__HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_UPDATE);

	/* Enable the Peripheral */
	__HAL_TIM_ENABLE(&htim1);

	/* Start DMA transfer */
	HAL_DMA_Start_IT(&hdma_tim1_up, (uint32_t) motion_dmabuf, (uint32_t) &GPIOB->BSRR, MOTION_DMA_BUFLEN);
}
