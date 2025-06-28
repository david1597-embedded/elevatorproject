/*
 * stepper.c
 *
 *  Created on: Jun 20, 2025
 *      Author: USER
 */
#include "stepper.h"

//0~7까지 받아 업데이트할 스텝을 결정
void stepMotor(uint8_t step)
{
	HAL_GPIO_WritePin(IN1_GPIO_Port, IN1_Pin, HALF_STEP_SEQ[step][0]);
	HAL_GPIO_WritePin(IN2_GPIO_Port, IN2_Pin, HALF_STEP_SEQ[step][1]);
	HAL_GPIO_WritePin(IN3_GPIO_Port, IN3_Pin, HALF_STEP_SEQ[step][2]);
	HAL_GPIO_WritePin(IN4_GPIO_Port, IN4_Pin, HALF_STEP_SEQ[step][3]);
}

