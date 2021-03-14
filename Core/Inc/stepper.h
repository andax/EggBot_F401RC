/*
 * stepper.h
 *
 *  Created on: Mar 11, 2021
 *      Author: Andreas
 */

#ifndef INC_STEPPER_H_
#define INC_STEPPER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

typedef struct
{
	int32_t duration;
	int32_t axis1;
	int32_t axis2;
} motion_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
extern SPI_HandleTypeDef hspi1;
extern int32_t Motor1_CurrentPos;
extern int32_t Motor2_CurrentPos;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/* USER CODE BEGIN EFP */
void Stepper_Init();
void Stepper_MotorEnable(int32_t enable1, int32_t enable2);
void Stepper_Move(int32_t duration, int32_t axis1_Delta,  int32_t axis2_Delta);
uint8_t Stepper_Active();

/* USER CODE END EFP */

/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* INC_STEPPER_H_ */
