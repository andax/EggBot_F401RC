/*
 * stepper.c
 *
 *  Created on: Mar 13, 2021
 *      Author: Andreas
 */

/* Includes ------------------------------------------------------------------*/
#include "servo.h"
#include "tim.h"
#include <stdbool.h>

/* Private variables ------------------------------------------------------------*/

bool penState = true;

// RC Servo variables
int32_t desired_pulse_value = 6000;
int32_t current_pulse_value = 6000;
int32_t max_pulse_value_rate = 100;

float pen_up_pulse_width = 1.78;
float pen_down_pulse_width = 1.45;

void Set_Servo_PWM(float pulse_width)
{
	// Ensure pulse width is between 0.5 - 2.5 ms
	if (pulse_width < 0.5)
	{
		pulse_width = 0.5;
	}
	if (pulse_width > 2.5)
	{
		pulse_width = 2.5;
	}

	// Update the desired pulse value
	desired_pulse_value = 60000*pulse_width/20.0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM9)
	{
		HAL_GPIO_WritePin(RC_Servo_PWM_GPIO_Port, RC_Servo_PWM_Pin, GPIO_PIN_SET);

		int32_t delta = desired_pulse_value - current_pulse_value;
		if (delta < -max_pulse_value_rate)
			delta = -max_pulse_value_rate;
		if (delta > max_pulse_value_rate)
			delta = max_pulse_value_rate;
		current_pulse_value += delta;
		__HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, current_pulse_value);
	}
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM9)
	{
		HAL_GPIO_WritePin(RC_Servo_PWM_GPIO_Port, RC_Servo_PWM_Pin, GPIO_PIN_RESET);
	}
}

void Servo_Init()
{
	  __HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_3, current_pulse_value);
	  HAL_TIM_PWM_Start_IT(&htim9, TIM_CHANNEL_3);
	  __HAL_TIM_ENABLE_IT(&htim9, TIM_IT_UPDATE | TIM_IT_CC1);
}

void SetPen(int32_t value)
{
	if (value)
	{
		Set_Servo_PWM(pen_up_pulse_width);
		penState = true;
	}
	else
	{
		Set_Servo_PWM(pen_down_pulse_width);
		penState = false;
	}
}

void TogglePen()
{
	SetPen(!penState);
}

void SetPenUpValue(int32_t value)
{
	pen_up_pulse_width = 2.778-value*0.000083;
	if(penState)
		SetPen(penState);
}

void SetPenDownValue(int32_t value)
{
	pen_down_pulse_width = 2.778-value*0.000083;
	if(!penState)
		SetPen(penState);
}

void SetPWMTest(int32_t value)
{
	Set_Servo_PWM(value/1000.0);
}

