# 🔵 STM32 기반 엘리베이터 프로젝트 (Elevator Control System)

## 🧰 기술 스택
![STM32](https://img.shields.io/badge/Board-STM32F411RE-blue)
![Language](https://img.shields.io/badge/Language-C-informational)
![Motor](https://img.shields.io/badge/Motor-Stepper-green)
![Algorithm](https://img.shields.io/badge/SCAN-algorithm-blue)


블로그를 통해 자세한 설명을 확인하세요!!

> **프로젝트 링크:** [Velog 블로그 링크](https://velog.io/@david1597/%EC%97%98%EB%A6%AC%EB%B2%A0%EC%9D%B4%ED%84%B0-%ED%94%84%EB%A1%9C%EC%A0%9D%ED%8A%B8-1)

## 📌 프로젝트 개요

이 프로젝트는 **STM32F411RE** 보드를 기반으로 실제 엘리베이터 시스템의 동작을 임베디드 환경에서 구현하는 것을 목표로 함
내부 요청, 외부 상승/하강 요청을 각각 구분하여 SCAN 알고리즘활용한 엘리베이터 알고리즘을  적용, 실제 동작 시간과 인터럽트 기반 요청 처리, UART 로그 기록 등을 통해 **현실적인 시뮬레이션 환경**을 조성을 해 엘리베이터의 실제 움직임과 로그의 비교를 통해 디버깅을 실시해 효율을 높임

---

## 플로우 차트

<img width="362" height="527" alt="image" src="https://github.com/user-attachments/assets/e44afc06-98b1-4e37-a6d9-6a561dc84834" />




## 🛠️ 사용 하드웨어 및 시스템 구성

![KakaoTalk_20250710_194846447](https://github.com/user-attachments/assets/102f81c8-7345-4421-a160-4adb7a451bf9)


- **MCU**: STM32F411RE (Nucleo 보드)
- **모터 제어**
  - 스테퍼 모터 (IN1: PC8, IN2: PB9, IN3: PB8, IN4: PC0)
  - 구동 전원: 12V
  - 타이머: TIM10 (프리스케일러: 99, 카운터 주기: 1914)
- **입력 장치**
  - 버튼: PB0 ~ PB6 (외부 인터럽트 EXTI)
- **층 감지 센서**
  - 조도 센서: PA10, PA11, PA12
- **통신**
  - UART 통신 (HUART)

---

## ⚙️ 시스템 알고리즘 및 동작 흐름

엘리베이터는 **SCAN 알고리즘**을 기반으로 하여 요청을 정방향 우선 순서로 처리함.  
내부, 외부 상승, 외부 하강 요청은 각각 큐로 관리되며, **현재 상태(IDLE, UP, DOWN)**와 **현재 층**에 따라 다르게 동작함.

요청처리는 다음과 같이 구분됩니다:
- **정지 중 동일 층 요청 처리**
- **정지 중 다른 층 요청 처리**
- **이동 중 현재 이동 방향 및 다음 층을 고려한 요청 처리 **

이동 중 시간은 `HAL_GetTick()`을 통해 측정되어 도착 및 정지를 제어합니다.

---

## 🧪 실시간 동작 로그 (UART 출력)


### ⚪1. 엘리베이터 대기 상태
```
current floor : [B1] -> next floor : [B1]   CURRENT STATE TO [IDLE]
State changed to [IDLE] after request processing 
NO REQUSESTS. current floor : B1 , current state : IDLE
current floor : [B1] -> next floor : [B1]   CURRENT STATE TO [IDLE]
State changed to [IDLE] after request processing 
NO REQUSESTS. current floor : B1 , current state : IDLE
```

### ⚪2. 엘리베이터 요청 발생
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

### ⚪3. 엘리베이터 이동 중 로그
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


---

## 📁 디렉토리 구조
```
Core/
├── Inc/                       # 헤더 파일
│   ├── button.h               # 버튼 처리 로직
│   ├── elevator.h             # 엘리베이터 알고리즘 정의
│   ├── gpio.h, main.h         # 시스템 설정
│   ├── photo.h                # 조도 센서 관련
│   ├── stepper.h              # 스테퍼 모터 제어
│   ├── tim.h, usart.h         # 타이머, UART 관련
│   └── stm32f4xx_it.h, conf.h # 인터럽트 처리
├── Src/                        # 소스 코드
│   ├── button.c
│   ├── elevator.c
│   ├── gpio.c, main.c
│   ├── photo.c, stepper.c
│   ├── stm32f4xx_it.c, msp.c
│   ├── tim.c, usart.c
│   └── syscalls.c, system_*.c
```

>하드웨어별로 헤더와 소스파일을 분리해 기능을 구현했다.
---

## 🎥 시나리오 1 데모 영상

---

## ✳️ 주요 기능 요약
- SCAN 알고리즘과 큐 기반 요청 처리를 이용한 실제 엘리베이터에 가까운 기능 구현
- 내부/ 외부요청을 분리해 관리
- 인터럽트를 이용하여 엘리베이터 구동에 방해받지 않고 실시간으로 입력 수용 기능 구현
- 스테퍼 모터를 이용한 엘리베이터의 기계적 구조 이해 및 요구사항에 맞춘 소프트웨어 제어 구현

---

## 🧠프로젝트의 주안점 및 고찰
- 단순 버튼 조작을 통한 모터 제어가 아닌 실제 엘리베이터의 구동을 구현하기 위한 알고리즘 작성 및 소프트웨어 코드의 직접 구현을 목표로 함
- 프로젝트의 요구사항에 맞추기 위한 스테퍼 모터 제어를 위한 실험적 파라미터 도출







