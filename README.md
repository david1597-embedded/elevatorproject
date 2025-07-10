# 🔵elevatorproject


![image](https://github.com/user-attachments/assets/e42e9b53-3697-4fa6-af3d-f6c2dbd50566)

엘리베이터 구동 관련 코드 게시

## 🔵현재 엘리베이터 하드웨어 구성

- 메인 MCU 
STM32 F411RE

- 주변 기기 핀 세팅 

    - 스테퍼 모터 스텝 주기 계산용 타이머 
      사용 타미어 : TIM10

       분주비 : 100 - 1 (시스템 클럭  100MHz)
      카운터 주기 : 1914 - 1


    - 버튼
        외부인터럽트 PB0 ~ PB6

    - 층 감지용 조도센서
        PA10, PA11, PA12






