/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SD_CARD_DET_Pin GPIO_PIN_13
#define SD_CARD_DET_GPIO_Port GPIOC
#define GPIO_0_Pin GPIO_PIN_0
#define GPIO_0_GPIO_Port GPIOC
#define GPIO_1_Pin GPIO_PIN_1
#define GPIO_1_GPIO_Port GPIOC
#define GPIO_2_Pin GPIO_PIN_2
#define GPIO_2_GPIO_Port GPIOC
#define GPIO_3_Pin GPIO_PIN_3
#define GPIO_3_GPIO_Port GPIOC
#define SPI1_NSS_Pin GPIO_PIN_4
#define SPI1_NSS_GPIO_Port GPIOA
#define Motor_1_SG_TST_Pin GPIO_PIN_4
#define Motor_1_SG_TST_GPIO_Port GPIOC
#define Motor_2_SG_TST_Pin GPIO_PIN_5
#define Motor_2_SG_TST_GPIO_Port GPIOC
#define Motor_1_Dir_Pin GPIO_PIN_12
#define Motor_1_Dir_GPIO_Port GPIOB
#define Motor_1_Step_Pin GPIO_PIN_13
#define Motor_1_Step_GPIO_Port GPIOB
#define Motor_2_Dir_Pin GPIO_PIN_14
#define Motor_2_Dir_GPIO_Port GPIOB
#define Motor_2_Step_Pin GPIO_PIN_15
#define Motor_2_Step_GPIO_Port GPIOB
#define Motor_1_En_Pin GPIO_PIN_6
#define Motor_1_En_GPIO_Port GPIOC
#define Motor_2_En_Pin GPIO_PIN_9
#define Motor_2_En_GPIO_Port GPIOA
#define RC_Servo_PWM_Pin GPIO_PIN_10
#define RC_Servo_PWM_GPIO_Port GPIOA
#define LED_R_Pin GPIO_PIN_4
#define LED_R_GPIO_Port GPIOB
#define LED_G_Pin GPIO_PIN_5
#define LED_G_GPIO_Port GPIOB
#define LED_B_Pin GPIO_PIN_6
#define LED_B_GPIO_Port GPIOB
#define User_Button_Pin GPIO_PIN_7
#define User_Button_GPIO_Port GPIOB
#define User_Button_EXTI_IRQn EXTI9_5_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
