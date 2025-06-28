/*
 * photo.c
 *
 *  Created on: Jun 11, 2025
 *      Author: USER
 */

#include "photo.h"

uint8_t zone_low = 0;
uint8_t zone_mid = 0;
uint8_t zone_high = 0;
//가려지면 1
uint8_t Get_3cm_photo_data(void)
{
	zone_low = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10);
	return zone_low;
}
uint8_t Get_9cm_photo_data(void)
{
	zone_mid = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11);
	return zone_mid;
}
uint8_t Get_15cm_photo_data(void)
{
	zone_high = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12);
	return zone_high;
}
