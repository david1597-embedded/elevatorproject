/*
 * button.h
 *
 *  Created on: Jun 10, 2025
 *      Author: USER
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_


#include "main.h"

#include "usart.h"

#include "gpio.h"


#include <stdlib.h>

#include <string.h>

#include <stdio.h>



#define BUTTON_COUNT 7

//지하 1층에 관련된 요청 받을지, 침수 시에는 요청 못 받음
extern uint8_t can_receive_req_B1F;

#endif /* INC_BUTTON_H_ */
