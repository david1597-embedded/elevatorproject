/*
 * elevator.h
 *
 *  Created on: Jun 10, 2025
 *      Author: USER
 */

#ifndef INC_ELEVATOR_H_
#define INC_ELEVATOR_H_

#include "main.h"
#include "button.h"
#include "tim.h"
#include "stepper.h"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define R2D 3
#define R1U 2
#define R1D 2
#define RBU 1

#define RF2 3
#define RF1 2
#define RB1 1

typedef enum {
    IDLE,
    DOWN,
    UP,
} ElevatorState;

typedef enum {
    B1 = 1,
    F1 = 2,
    F2 = 3
} ElevatorFloor;

// 전역 배열 선언
extern uint8_t internal_requests[3];
extern uint8_t up_requests[3];
extern uint8_t down_requests[3];
extern uint8_t can_operate_flag;
extern uint8_t current_floor;       // 현재 층 (정적 변수로 외부 접근 제한)
extern uint8_t next_floor;

uint8_t *get_internal_requests(void);
uint8_t *get_up_requests(void);
uint8_t *get_down_requests(void);
void set_currentfloor(void);
uint8_t set_state(void);

uint8_t check_request(uint8_t *requests);
void door_open_close();
void operate_elevator_up(void);
void operate_elevator_down(void);
void operate_elevator_stop(void);
void update_step(uint16_t step_counter);

uint8_t find_request_index(uint8_t *requests, uint8_t floor);
uint8_t find_request_name(uint8_t *requests, uint8_t floor);
uint8_t find_request_adove(uint8_t *requests, uint8_t floor);
uint8_t find_request_below(uint8_t *requests, uint8_t floor);

void elevator_process(void);
void push_requests(uint8_t *requests, uint8_t operator);
void remove_request(uint8_t *requests, uint8_t floor, uint8_t is_processed);
uint8_t choose_closest_floor(uint8_t * requests);
uint8_t choose_next_floor_in_direction(uint8_t *internal, uint8_t *up, uint8_t *down, uint8_t current_floor, ElevatorState direction);
#endif /* INC_ELEVATOR_H_ */
