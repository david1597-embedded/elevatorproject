/*
 * stepper.h
 *
 *  Created on: Jun 20, 2025
 *      Author: USER
 */

#ifndef INC_STEPPER_H_
#define INC_STEPPER_H_

#include "main.h"

#define IN1_GPIO_Port  GPIOC
#define IN1_Pin		   GPIO_PIN_8
#define IN2_GPIO_Port  GPIOB
#define IN2_Pin		   GPIO_PIN_9
#define IN3_GPIO_Port  GPIOB
#define IN3_Pin		   GPIO_PIN_8
#define IN4_GPIO_Port  GPIOC
#define IN4_Pin		   GPIO_PIN_9

static const uint8_t HALF_STEP_SEQ[8][4] =
{
		{1, 0, 0, 0},
		{1, 1, 0, 0},
		{0, 1, 0, 0},
		{0, 1, 1, 0},
		{0, 0, 1, 0},
		{0, 0, 1, 1},
		{0, 0, 0, 1},
		{1, 0, 0, 1}
};

void stepMotor(uint8_t);
#endif /* INC_STEPPER_H_ */
