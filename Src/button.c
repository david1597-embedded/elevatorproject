/*
 * button.c
 *
 *  Created on: Jun 10, 2025
 *      Author: USER
 */

#include "button.h"
#include "elevator.h"

static uint32_t last_press_time[BUTTON_COUNT] = {0};
static const uint32_t debounce_delay = 300;

static const uint16_t button_pins[BUTTON_COUNT] = {
    BUTTON_2F_DOWN_Pin, BUTTON_1F_UP_Pin, BUTTON_1F_DOWN_Pin, BUTTON_B1F_UP_Pin,
    BUTTON_2F_SELECT_Pin, BUTTON_1F_SELECT_Pin, BUTTON_B1F_SELECT_Pin
};

uint8_t can_receive_req_B1F = 1;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
     for(uint8_t i = 0; i<BUTTON_COUNT; i++)
     {
    	 if(GPIO_Pin == button_pins[i])
    	 {

    		 if(HAL_GetTick() - last_press_time[i] > debounce_delay)
    		 {
    			 HAL_UART_Transmit(&huart2, (uint8_t*)"Interrupt Triggered! \n",21,100);
    			 uint8_t floor;
    			 uint8_t *target_requests;
    			 char msg[64];

    			 switch (GPIO_Pin) {
					 case BUTTON_2F_DOWN_Pin:
						 floor = 3; // F2
						 target_requests = down_requests;
						 break;
					 case BUTTON_1F_UP_Pin:
						 floor = 2; // F1
						 target_requests = up_requests;
						 break;
					 case BUTTON_1F_DOWN_Pin:
						 floor = 2; // F1
						 target_requests = down_requests;
						 break;
					 case BUTTON_B1F_UP_Pin:
						 if (!can_receive_req_B1F)
						{
							printf("B1F FLOODING!!!! \r\n");
							return; // B1F 요청 차단
						}
						 floor = 1; // B1
						 target_requests = up_requests;
						 break;
					 case BUTTON_2F_SELECT_Pin:
						 floor = 3; // F2
						 target_requests = internal_requests;
						 break;
					 case BUTTON_1F_SELECT_Pin:
						 floor = 2; // F1
						 target_requests = internal_requests;
						 break;
					 case BUTTON_B1F_SELECT_Pin:
						 if (!can_receive_req_B1F)
						 {

							printf("B1F FLOODING!!!! \r\n");
							return; // B1F 요청 차단
						 }
						 floor = 1; // B1
						 target_requests = internal_requests;
						 break;
					 default:
						 return;
				  }
    			  int request_index = find_request_name(target_requests, floor);
				  if (request_index == 1)
				  {
				  	remove_request(target_requests, floor,0);
				  }
				  else
				  {
					push_requests(target_requests, floor);
				  }

				  sprintf(msg, "Current state - up: [%d,%d,%d], down: [%d,%d,%d], internal: [%d,%d,%d] \r\n",
						up_requests[0], up_requests[1], up_requests[2],
						down_requests[0], down_requests[1], down_requests[2],
						internal_requests[0], internal_requests[1], internal_requests[2]);
				  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

				  last_press_time[i] = HAL_GetTick();
			    }
				break;
    		 }
    	 }
    }


