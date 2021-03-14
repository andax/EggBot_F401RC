/*
 * servo.h
 *
 *  Created on: Mar 13, 2021
 *      Author: Andreas
 */

#ifndef INC_SERVO_H_
#define INC_SERVO_H_
#include "main.h"

void Set_Servo_PWM(float pulse_width);
void Servo_Init();
void SetPenUpValue(int32_t value);
void SetPenDownValue(int32_t value);
void SetPen(int32_t value);
void TogglePen();

void SetPWMTest(int32_t value);


#endif /* INC_SERVO_H_ */
