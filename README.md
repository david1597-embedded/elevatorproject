# 🔵elevatorproject


![image](https://github.com/user-attachments/assets/e42e9b53-3697-4fa6-af3d-f6c2dbd50566)

엘리베이터 구동 관련 코드 게시

## 🔵현재 엘리베이터 하드웨어 구성

![KakaoTalk_20250710_194846447](https://github.com/user-attachments/assets/8ea75d5b-dbc3-486e-83aa-f756289dd65e)


- 메인 MCU 
STM32 F411RE

- 주변 기기 핀 세팅 

    - 스테퍼 모터 스텝 주기 계산용 타이머 
       사용 타이머 : TIM10

       분주비 : 100 - 1 (시스템 클럭  100MHz)

       카운터 주기 : 1914 - 1

      스텝 계산용 GPIO
      IN1 : PC8
      IN2 : PB9
      IN3 : PB8
      IN4 : PC0

      스텝 모터 드라이버 입력 전원
      12V

    - 버튼
        외부인터럽트 PB0 ~ PB6

    - 층 감지용 조도센서
        PA10, PA11, PA12

## 엘리베이터 구동 알고리즘

엘리베이터는 SCAN 알고리즘을 통해 이동방향을 우선으로 하여 외부인터럽트를 통해 입력된 요청들을 바탕으로 움직임이 결정된다.

엘리베이터의 요청은 내부, 외부 하강, 외부 상승 버튼을 나누어 세 개의 큐 변수로 관리된다.

엘리베이터 구동 시간을 HAL_GetTick()함수를 이용해 측정하여 시간 별로 요청을 처리하는 로직을 만들어 다음 층에서 멈출지 지나갈지를 결정한다.

요청은 이동중, 정지중을 나누어 다르게 처리한다.


## 실제 엘리베이터 구동 로그 (HUART로 확인)

### 1. 엘리베이터 대기 상태
```
current floor : [B1] -> next floor : [B1]   CURRENT STATE TO [IDLE]
State changed to [IDLE] after request processing 
NO REQUSESTS. current floor : B1 , current state : IDLE
current floor : [B1] -> next floor : [B1]   CURRENT STATE TO [IDLE]
State changed to [IDLE] after request processing 
NO REQUSESTS. current floor : B1 , current state : IDLE
```

### 2. 엘리베이터 요청 발생
```
Interrupt Triggered! Pushed request 1 to internal
After push - [1,0,0]
Current state - up: [0,0,0], down: [0,0,0], internal: [1,0,0] 
====START PROCESSING SAME FLOOR REQUESTS====
current floor : [B1] -> next floor : [B1]   CURRENT STATE TO [IDLE]
Popped request 1 from internal
After remove - [0,0,0]
CURRENT FLOOR INTERNAL REQUESTS PROCESSE COMPLETE !!!! 
====START PROCESSING OTHER FLOORS REQUESTS====
State changed to [IDLE] after request processing 
current floor : [B1] -> next floor : [B1]   CURRENT STATE TO [IDLE]
No remaining requests...
```

### 3. 엘리베이터 이동 중 로그
```
Interrupt Triggered! Pushed request 2 to internal
After push - [2,0,0]
Current state - up: [0,0,0], down: [0,0,0], internal: [2,0,0] 
====START PROCESSING SAME FLOOR REQUESTS====
====START PROCESSING OTHER FLOORS REQUESTS====
State changed to [UP] after request processing
current floor : [B1] -> next floor : [F1]   CURRENT STATE TO [UP]
Start Elevator up 
OTHER FLOOR INTERNAL REQUESTS PROCESS COMPLETE [B1F or F2] !!! 
Elevatring going [UP] : Elasped_time [1089] ms 
Elevatring going [UP] : Elasped_time [2141] ms 

Elevatring going [UP] : Elasped_time [3193] ms 
Elevatring going [UP] : Elasped_time [4245] ms 
Elevatring going [UP] : Elasped_time [5297] ms 
Elevatring going [UP] : Elasped_time [6349] ms 
Elevatring going [UP] : Elasped_time [7401] ms 
Elevatring going [UP] : Elasped_time [8453] ms 
Elevatring going [UP] : Elasped_time [9505] ms 
Eleavtor will [Stop] at current floor :[F2] : Elasped_time [10557] ms 
State changed to [IDLE] after request processing 
Popped request 2 from internal
After remove - [0,0,0]
Removed internal request for floor F1
Elevator stopped at [F1] : Elapsed time [0] 
Door opended... 
NO REQUSESTS. current floor : F1 , current state : IDLE
current floor : [F1] -> next floor : [F1]   CURRENT STATE TO [IDLE]
State changed to [IDLE] after request processing 
```






