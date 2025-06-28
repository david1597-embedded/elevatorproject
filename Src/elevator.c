/*
 * elevator.c
 *
 *  Created on: Jun 10, 2025
 *      Author: USER
 */

#include "elevator.h"
#include "photo.h"
uint8_t internal_requests[3] = {0}; // 내부 요청 배열 (최대 3개 층: B1, F1, F2)
uint8_t up_requests[3] = {0};       // 위로 가는 외부 요청 배열 (최대 3개)
uint8_t down_requests[3] = {0};     // 아래로 가는 외부 요청 배열 (최대 3개)
uint8_t current_floor;              // 현재 층 (정적 변수로 외부 접근 제한)
uint8_t prev_floor;                 // 출발 층 설정
uint8_t next_floor;                 // 다음 목표 층 (정적 변수로 외부 접근 제한)
uint32_t moving_time=0;
uint8_t moving = 0;                 // 1:이동중 0: 정지
uint16_t total_step = 3911;
static uint16_t timer_counter = 0;
static uint16_t step_counter = 0;
ElevatorState state = IDLE;         // 엘리베이터 상태 (IDLE: 정지, UP: 상승, DOWN: 하강)
uint32_t start_time;
uint32_t elapsed_time;

// 층을 문자열로 변환하는 헬퍼 함수
// 매개변수: floor - 층 번호 (B1, F1, F2)
// 반환값: 해당 층의 문자열 표현
const char *floor_to_str(uint8_t floor) {
    switch (floor) {
        case B1: return "B1";   // 지하 1층
        case F1: return "F1";   // 1층
        case F2: return "F2";   // 2층
        default: return "Unknown"; // 알 수 없는 층
    }
}

// 상태를 문자열로 변환하는 헬퍼 함수
// 매개변수: s - 엘리베이터 상태 (IDLE, UP, DOWN)
// 반환값: 해당 상태의 문자열 표현
const char *state_to_str(ElevatorState s) {
    switch (s) {
        case IDLE: return "IDLE";   // 정지 상태
        case UP: return "UP";       // 상승 상태
        case DOWN: return "DOWN";   // 하강 상태
        default: return "Unknown";  // 알 수 없는 상태
    }
}

// 내부 요청 배열 포인터 반환
// 반환값: internal_requests 배열의 포인터
uint8_t *get_internal_requests(void)
{
    return internal_requests;
}

// 위로 가는 요청 배열 포인터 반환
// 반환값: up_requests 배열의 포인터
uint8_t *get_up_requests(void)
{
    return up_requests;
}

// 아래로 가는 요청 배열 포인터 반환
// 반환값: down_requests 배열의 포인터
uint8_t *get_down_requests(void)
{
    return down_requests;
}

void set_currentfloor(void)
{
	if(Get_3cm_photo_data() == 1 && Get_9cm_photo_data() == 0 && Get_15cm_photo_data() == 0)
	{
		current_floor = B1;
	}

	else if(Get_3cm_photo_data() == 0 && Get_9cm_photo_data() == 1 && Get_15cm_photo_data() == 0)
	{
		current_floor = F1;
	}

	else if(Get_3cm_photo_data() == 0 && Get_9cm_photo_data() == 0 && Get_15cm_photo_data() == 1)
	{
		current_floor = F2;
	}
}

 uint8_t set_state(void)
{
	if(current_floor == next_floor)
	{
		state = IDLE;
		printf("State changed to [%s] after request processing \n",state_to_str(state));
	}
	 else if (current_floor < next_floor)
	{
	    state = UP;
		printf("State changed to [%s] after request processing\n",state_to_str(state));
	}
	else if (current_floor > next_floor)
	{
	    state = DOWN;
		printf("State changed to [%s] after request processing\n",state_to_str(state));
	}
	return state;
}

// 요청 배열에 요청이 있는지 확인
// 매개변수: requests - 확인할 요청 배열
// 반환값: 1 (요청 있음), 0 (요청 없음)
uint8_t check_request(uint8_t *requests)
{
    for (int i = 0; i < 3; i++)
    {
        if (requests[i] != 0)
        { // 요청이 0이 아니면 (즉, 층 요청이 존재)
            return 1;
        }
    }
    return 0;
}

//문 열고 닫기
//1초 대기 후 현재 층에서 문 열고 닫는 동작 출력

void door_open_close(void)
{
	printf("Door opended... \r\n");
	//문열기
	HAL_Delay(5000);
	//문닫기
	HAL_Delay(1000); // 1초 대기 (문 열고 닫는 시간)
}

void operate_elevator_move(void)
{
	//타이머9를 시작하고
	HAL_TIM_Base_Start_IT(&htim10);
	start_time = HAL_GetTick();
	moving = 1;
	step_counter = 0;
}

void operate_elevator_stop()
{
	//HAL_TIM_Base_Stop_IT(&htim9);
}
void update_step(uint16_t step_counter)
{
	if(moving == 1)
	{
		uint8_t step = 0;
		if(state==UP)
		{
			step = 7 - (step_counter % 8);
		}
		else if(state == DOWN)
		{
			step = step_counter % 8;
		}
		stepMotor(step);
	}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//timer_counter : to count 1.913ms
	//step_counter
	//Timer interrupt will be se when 1.913ms elapsed...
	if(htim ->Instance == TIM10)
	{
		if(moving == 1)
		{
			if(step_counter >= 7199)
			{
				moving = 0;
				step_counter = 0;
				HAL_TIM_Base_Stop_IT(&htim10); // 타이머 중지
	//			HAL_TIM_Base_Stop(&htim10);
			}
			update_step(step_counter);
			step_counter ++ ;
		}
	}
}

// 특정 층 요청의 인덱스 찾기
// 매개변수: requests - 요청 배열, floor - 찾을 층 번호
// 반환값: 요청 인덱스 (없으면 -1)
uint8_t find_request_index(uint8_t *requests, uint8_t floor)
{
    for (int i = 0; i < 3; i++)
    {
        if (requests[i] == floor)
        {
            return i; // 해당 층의 인덱스 반환
        }
    }
    return -1; // 요청이 없으면 -1 반환
}

//특정 층 요청이 있는지 확인
uint8_t find_request_name(uint8_t *requests, uint8_t floor)
{
    for (int i = 0; i < 3; i++)
    {
        if (requests[i] == floor)
        {
            return 1; // 해당 층의 요청 있으면 1 반환
        }
    }
    return 0;
}

//현재 층보다 높은 층의 요청이 있는지 확인 후 인덱스 반환
uint8_t find_request_adove(uint8_t *requests, uint8_t floor)
{
    for (int i = 0; i < 3; i++)
    {
        if(requests[i] !=0)
        {
            if (requests[i] > floor)
            {
                return 1; // 해당 층의 요청 있으면 1 반환
            }
        }
    }
    return 0; // 요청이 없으면 -1 반환
}

//현재 층보다 높은 층의 요청이 있는지 확인 후 인덱스 반환
uint8_t find_request_below(uint8_t *requests, uint8_t floor)
{
    for (int i = 0; i < 3; i++)
    {
        if(requests[i] !=0)
        {
            if (requests[i] < floor)
            {
                return 1; // 해당 층의 요청 있으면 1 반환
            }
        }
    }
    return 0; // 요청이 없으면 -1 반환
}


// 외부 인터럽트로 요청 추가
// 매개변수: requests - 추가할 요청 배열, operator - 추가할 층 번호
void push_requests(uint8_t *requests, uint8_t operator)
{
    uint8_t count = 0;
    for (int i = 0; i < 3; i++)
    {
        if (requests[i] != 0)
        {
            count++; // 현재 요청 개수 계산
        }
        if (requests[i] == operator)
        {
            return; // 중복 요청이면 추가하지 않음
        }
    }
    if (count < 3)
    {
       // 배열이 꽉 차지 않았으면
        requests[count] = operator; // 빈 자리에 요청 추가
        char msg[64];
        sprintf(msg, "Pushed request %d to %s\n", operator,
                (requests == internal_requests) ? "internal" :
                (requests == up_requests) ? "up" : "down");
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

        sprintf(msg, "After push - [%d,%d,%d]\n", requests[0], requests[1], requests[2]);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
    }
}

// 요청 제거 (처리 완료 또는 버튼 토글)
// 매개변수: requests - 요청 배열, floor - 제거할 층, is_processed - 처리 완료 여부 (1: 완료, 0: 토글)
void remove_request(uint8_t *requests, uint8_t floor, uint8_t is_processed)
{
    int index = find_request_index(requests, floor);
    if (index != -1)
    { // 요청이 존재하면
        for (int i = index; i < 2; i++)
        {
            requests[i] = requests[i + 1]; // 뒤의 요소를 앞으로 이동
        }
        requests[2] = 0; // 마지막 요소 초기화
        char msg[64];
        if (is_processed)
        {
            sprintf(msg, "Popped request %d from %s\n", floor,
                    (requests == internal_requests) ? "internal" :
                    (requests == up_requests) ? "up" : "down");
        }
        else
        {
            sprintf(msg, "Request %d removed from %s\n", floor,
                    (requests == internal_requests) ? "internal_requests" :
                    (requests == up_requests) ? "up_requests" : "down_requests");
        }
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

        sprintf(msg, "After remove - [%d,%d,%d]\n", requests[0], requests[1], requests[2]);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
    }
}
//정지상태에서 요청 처리
//요청은 처리를 하나 끝내고 다시 요청을 확인

uint8_t choose_closest_floor(uint8_t * requests)
{
   uint8_t closest_floor=0xFF;
    for(uint8_t i=0;i<3;i++)
    {
       if(requests[i] != 0)
       {
          if(closest_floor > abs(requests[i] - current_floor))
          {
             closest_floor = requests[i];
          }
       }
    }
    return closest_floor;
}

uint8_t choose_next_floor_in_direction(uint8_t *internal, uint8_t *up, uint8_t *down, uint8_t current_floor, ElevatorState direction) {
    uint8_t next = 0xFF; // 초기값: 유효하지 않은 층
    if (direction == UP)
    {
        for (int i = 0; i < 3; i++) {
            if (internal[i] != 0 && internal[i] > current_floor && (next == 0xFF || internal[i] < next)) next = internal[i];
            if (up[i] != 0 && up[i] > current_floor && (next == 0xFF || up[i] < next)) next = up[i];
            if (down[i] != 0 && down[i] > current_floor && (next == 0xFF || down[i] < next)) next = down[i];
        }
    }
    else if (direction == DOWN) {
        for (int i = 0; i < 3; i++) {
            if (internal[i] != 0 && internal[i] < current_floor && (next == 0xFF || internal[i] > next)) next = internal[i];
            if (up[i] != 0 && up[i] < current_floor && (next == 0xFF || up[i] > next)) next = up[i];
            if (down[i] != 0 && down[i] < current_floor && (next == 0xFF || down[i] > next)) next = down[i];
        }
    }
    return next == 0xFF ? current_floor : next; // 요청 없으면 현재 층 반환
}

void print_message()
{
    printf("current floor : [%s] -> next floor : [%s]   CURRENT STATE TO [%s]\r\n", floor_to_str(current_floor), floor_to_str(next_floor), state_to_str(state));
}

//요청 배열의 원소가 0이라면 요청이 없는 것임
//요청 배열의 원소가 1이면 B1F, 2이면 F1, 3이면 F2임
//요청 큐는 외부인터럽트로 언제든지 push pull이 가능함.
void request_process_STOPPED(void)
{
    //현재 층 설정
    //상태 설정은 요청 처리 로직에서 다음 층을 선택하는 것으로 함.
    set_currentfloor();
    if(!check_request(internal_requests) && !check_request(up_requests) && !check_request(down_requests))
    {
      printf("NO REQUSESTS. current floor : %s , current state : %s\r\n", floor_to_str(current_floor), state_to_str(state));
      print_message();
      next_floor = current_floor;
      set_state();
      return ;
    }

    printf("====START PROCESSING SAME FLOOR REQUESTS====\r\n");
    // 같은 층 요청 처리여부 확인하는 flag
    // 현재 층에 관한 처리를 먼저 진행함.
    uint8_t same_floor_request_complete_flag = 0;
    //내부요청이 있는 경우
    if(check_request(internal_requests))
    {
        for(uint8_t i = 0 ; i< 3 ;i++)
        {
           if(internal_requests[i] == current_floor)
           {
                next_floor = current_floor;
                print_message();


                //현재 층에 관련된 모든 요청 배열에서 요청 큐 삭제
                if(check_request(internal_requests) == 1)
                {
                    remove_request(internal_requests, current_floor, 1);
                }
                if(check_request(up_requests) == 1 )
                {
                    remove_request(up_requests, current_floor, 1);
                }
                if(check_request(down_requests) == 1)
                {
                    remove_request(down_requests, current_floor, 1);
                }
                printf("CURRENT FLOOR INTERNAL REQUESTS PROCESSE COMPLETE !!!! \r\n");
            }
        }
    }
       //내부요청은 없고 외부요청만 있는 경우
    else if(check_request(up_requests) || check_request(down_requests))
    {
        for(uint8_t i = 0 ; i< 3 ;i++)
        {
            if(up_requests[i] == current_floor || down_requests[i] == current_floor)
            {
                next_floor = current_floor;
                print_message();
                door_open_close();

                //현재 층에 관련된 모든 요청 배열에서 요청 큐 삭제
                if(check_request(up_requests) == 1 )
                {
                    remove_request(up_requests, current_floor, 1);
                }
                if(check_request(down_requests) == 1)
                {
                    remove_request(down_requests, current_floor, 1);
                }
                printf("CURRENT FLOOR EXTERANL REQUESTS PROCESSE COMPLETE !!!! \r\n");
            }
        }
    }
    //둘 다 없는 경우
    else
    {
        next_floor = current_floor;
        printf("No same floor internal requests ... !! \r\n");

    }
    same_floor_request_complete_flag = 1;
    //아래 요청 처리 로직에서는 현재 층과 관련된 요청이 없어야함
    printf("====START PROCESSING OTHER FLOORS REQUESTS====\r\n");
//    if(same_floor_request_complete_flag == 0)
//    {
//        printf("Something went wrong.... abort elevator ... !! \r\n");
//    }
    if(same_floor_request_complete_flag == 1)
    {
        if(check_request(internal_requests))
        {
            if(current_floor == F1)
            {
                //현재 층이 1층이라면 내부요청 중 가장 먼저 눌린 요청을 먼저 처리함
                //요청 배열의 원소가 0이라면 요청이 없는 것임
                //요청 배열의 원소가 1이면 B1F, 2이면 F1, 3이면 F2임
                prev_floor = current_floor;
                next_floor = internal_requests[0];
                set_state();
                print_message();
                if(state == UP)
                {
                    //엘리베이터 구동 후엔 아래 코드 확인 필요 없음
                	operate_elevator_move();
                    printf("Start Elevator up \r\n");
                }
                else if( state == DOWN)
                {
                    //엘리베이터 구동 후엔 아래 코드 확인 필요 없음
                	operate_elevator_move();
                    printf("Start Elevator down \r\n");
                }
                printf("OTHER FLOOR INTERNAL REQUESTS PROCESS COMPLETE [F1] !!! \r\n");
            }
            else
            {
                //만약 지하1층인데 2층 내부요청 1층 외부요청이 공존한다면?? 1층으로 가야함. 모든 요청배열 들을 검사하여 가장 가까운 층으로 가기
                uint8_t closest_floor_internal = choose_closest_floor(internal_requests);
                uint8_t closest_floor_up       = choose_closest_floor(up_requests);
                uint8_t closest_floor_down     = choose_closest_floor(down_requests);
                prev_floor = current_floor;
                next_floor = (closest_floor_internal < closest_floor_up ? closest_floor_internal : closest_floor_up) < closest_floor_down ?
                (closest_floor_internal < closest_floor_up ? closest_floor_internal : closest_floor_up) : closest_floor_down;
                set_state();
                print_message();
                if(state == UP)
                {
                    //엘리베이터 구동 후엔 아래 코드 확인 필요 없음
                	operate_elevator_move();
                    printf("Start Elevator up \r\n");
                }
                else if( state == DOWN)
                {
                    //엘리베이터 구동 후엔 아래 코드 확인 필요 없음
                	operate_elevator_move();
                    printf("Start Elevator down \r\n");
                }
                printf("OTHER FLOOR INTERNAL REQUESTS PROCESS COMPLETE [B1F or F2] !!! \r\n");
            }
        }
        //내부요청 없고 외부 요청만 있다면 외부요청 중 가장 가까운 층을 찾아 이동함.
        else if(check_request(up_requests) || check_request(down_requests))
        {
            uint8_t closest_floor_up;
            uint8_t closest_floor_down ;
            if(current_floor == F1)
            {
                //1층보다 위층의 요청이 있는지 확인
                if(find_request_adove(up_requests, current_floor) || find_request_adove(down_requests, current_floor))
                {
                    closest_floor_up = choose_closest_floor(up_requests);
                    closest_floor_down = choose_closest_floor(down_requests);
                    next_floor = closest_floor_up < closest_floor_down ? closest_floor_up : closest_floor_down;
                    set_state();
                    print_message();
                }
                //1층보다 아래층의 외부요청만 있다면..
                else
                {
                    closest_floor_up = choose_closest_floor(up_requests);
                    closest_floor_down = choose_closest_floor(down_requests);
                    next_floor = closest_floor_up < closest_floor_down ? closest_floor_up : closest_floor_down;
                    set_state();
                    print_message();
                }

                prev_floor = current_floor;
                if(state == UP)
                {
                    //엘리베이터 구동 후엔 아래 코드 확인 필요 없음
                	operate_elevator_move();
                    printf("Start Elevator up \r\n");
                }
                else if( state == DOWN)
                {
                    //엘리베이터 구동 후엔 아래 코드 확인 필요 없음
                	operate_elevator_move();
                    printf("Start Elevator down \r\n");
                }
                printf("OTHER FLOOR EXTERNAL REQUESTS PROCESS COMPLETE [F1] !!! \r\n");
                //3층 짜리 건물이라 가능함. 더 높은 층의 건물이라면 중간 층 요청에 대한 로직을 다시 작성해야함...
            }
            else
            {
                closest_floor_up = choose_closest_floor(up_requests);
                closest_floor_down = choose_closest_floor(down_requests);
                prev_floor = current_floor;
                next_floor = closest_floor_up < closest_floor_down ? closest_floor_up : closest_floor_down;
                set_state();
                print_message();

                if(state == UP)
                {
                    //엘리베이터 구동 후엔 아래 코드 확인 필요 없음
                	operate_elevator_move();
                    printf("Start Elevator up \r\n");
                }
                else if( state == DOWN)
                {
                    //엘리베이터 구동 후엔 아래 코드 확인 필요 없음
                	operate_elevator_move();
                    printf("Start Elevator down \r\n");
                }
                printf("OTHER FLOOR EXTERNAL REQUESTS PROCESS COMPLETE [B1F or F2] !!! \r\n");
            }
        }
        //같은 층 요청 모두 처리 후 남아 있는 요청이 없는 경우
        else
        {
            set_state();
            print_message();
            printf("No remaining requests... \r\n");
            return;
        }
    }

}


void request_process_MOVING(void)
{
    //엘리베이터가 동작되는 시점에 TIMER가 켜짐 해당 타이머로 구동시간을 측정해야함
    //6초가되는시점에 정지해야하는 경우가 있고 지나가는 경우가 있음
    //flag로 처리해 지나가는 로직을

	elapsed_time = HAL_GetTick() - start_time;
    static uint8_t should_pass = 0;

    if(elapsed_time<10000)
    {
        printf("Elevatring going [%s] : Elasped_time [%lu] ms \r\n",state_to_str(state),elapsed_time);
        set_currentfloor();
    }
    //요청 재확인
    else if(elapsed_time>=10000 && elapsed_time<11200)
    {
        //현재층 설정
        set_currentfloor();
        //현재 층에 대한 어떠한 요청이라도 있으면
		if(find_request_name(internal_requests, current_floor) || find_request_name(up_requests, current_floor) || find_request_name(down_requests, current_floor))
		{
			//정지 플래그
			should_pass = 0;
		}
		else
		{
            //현재층에 대한 어떤 요청도 없으면
			if(state == UP)
            {
                //현재 층보다 높은 층의 요청이 있다면
                if(find_request_adove(internal_requests, current_floor) ||find_request_adove(up_requests, current_floor) || find_request_adove(down_requests, current_floor) )
                {
                	printf("Requests adove from current floor [%s] \r\n", floor_to_str(current_floor));
                    should_pass = 1;
                }
                else
                {
                	should_pass = 0;
                }
            }

			else if(state == DOWN)
			{
				//현재 층보다 낮은 층의 요청이 있다면
				if(find_request_below(internal_requests, current_floor) ||find_request_below(up_requests, current_floor) || find_request_below(down_requests, current_floor) )
				{
					printf("Requests below from current floor [%s] \r\n", floor_to_str(current_floor));
					should_pass = 1;
				}
				else
				{
					should_pass = 0;
				}
			}
		}
        printf("Eleavtor will [%s] at current floor :[%s] : Elasped_time [%lu] ms \r\n", should_pass == 0 ? "Stop" : "Pass" , floor_to_str(current_floor), elapsed_time);
    }
//    else if(elapsed_time >= 11200 &&elapsed_time < 12000)
//    {
//    	//현재 층에 대한 요청이 있는지 최종적으로 검사
//    	if(find_request_name(internal_requests, current_floor) || find_request_name(up_requests, current_floor) || find_request_name(down_requests, current_floor))
//		{
//			//정지 플래그
//			should_pass = 0;
//		}
//    }
    else if(elapsed_time >= 12000)
    {
    	if(should_pass == 0)
		{
			//엘리베이터 정지(스테퍼 모터 구동하는 타이머 정지)
			if(prev_floor != current_floor )
			{
				elapsed_time = 0;
				operate_elevator_stop();
				next_floor = current_floor;
				set_state();
				//현재 층에 관련된 모든 요청 배열에서 요청 큐 삭제
				if(find_request_name(internal_requests, current_floor))
				{
					remove_request(internal_requests, current_floor, 1);
					printf("Removed internal request for floor %s\r\n", floor_to_str(current_floor));
				}
				if(find_request_name(up_requests, current_floor))
				{
					remove_request(up_requests, current_floor, 1);
					printf("Removed up request for floor %s\r\n", floor_to_str(current_floor));
				}
				if(find_request_name(down_requests, current_floor))
				{
					remove_request(down_requests, current_floor, 1);
					printf("Removed down request for floor %s\r\n", floor_to_str(current_floor));
				}
				printf("Elevator stopped at [%s] : Elapsed time [%lu] \r\n", floor_to_str(current_floor),elapsed_time);
			}
			door_open_close();
		}
		else
		{
			if(state == UP)
			{
				prev_floor = current_floor ;
				next_floor = current_floor + 1;
				set_state();
				printf("elevator keep going UP to [%s] \r\n",floor_to_str(next_floor));
				operate_elevator_move();

			}
			else if(state == DOWN)
			{
				prev_floor = current_floor;
				next_floor = current_floor -1;
				set_state();
				printf("elevator keep going DOWN to [%s] \r\n",floor_to_str(next_floor));
				operate_elevator_move();
			}

			should_pass = 0;

		}

    }

}


// 엘리베이터 동작 관리
void elevator_process(void)
{
    set_currentfloor();

    if (state == IDLE)
    {
       request_process_STOPPED();
    }
    else if(state == UP || state ==DOWN)
    {
       request_process_MOVING();
    }
}

