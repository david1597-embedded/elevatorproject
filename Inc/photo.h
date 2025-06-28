/*
 * photo.h
 *
 *  Created on: Jun 11, 2025
 *      Author: USER
 */

#ifndef INC_PHOTO_H_
#define INC_PHOTO_H_

#include "main.h"
#include "gpio.h"
#include "usart.h"

uint8_t Get_3cm_photo_data(void);
uint8_t Get_9cm_photo_data(void);
uint8_t Get_15cm_photo_data(void);

#endif /* INC_PHOTO_H_ */
