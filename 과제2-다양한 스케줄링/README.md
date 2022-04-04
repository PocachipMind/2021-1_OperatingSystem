# 과제 2
2-1. Multi-level queues with Real-time class
<br>
2-2. SRJF Scheduling
<br>
2-3. Round-Robin Scheduling
<br>
2-4. Evaluation
<br>
과제2 설명.pdf를 읽으시면 조금 더 자세한 과제설명을 알 수 있습니다.
## test2.bin
#### 예시 프로세스 바이너리 파일.
보고서에선 이 바이너리 파일을 기준으로 코드를 실행합니다.
<br>
자세한 프로세스 정보는 과제2 설명.pdf에 나와있습니다.
## os2-1.c
#### Multi-level queues with Real-time class.
stdin으로부터 이진파일 형태의 프로세스 정보를 입력받습니다.
<br>
그 프로세스를 Multi-level로 스케줄링 하도록 구현합니다.
<br>
Ready queue를 세 개의 계층적 스케줄링 클래스로 구분하고, 각각의 클래스별로 queue를 따로 관리하며, 큐에 대해 Priority-based scheduling 사용합니다.
<br>
세 개의 계층적 큐 : Real-time class / Normal class / Idle class
## os2-2.c
#### SRJF Scheduling.
stdin으로부터 이진파일 형태의 프로세스 정보를 입력받습니다.
<br>
그 프로세스를 SRJF로 스케줄링 하도록 구현합니다.
<br>
Normal class queue의 프로세스 중, 현재 진행중인 CPU operation을 비교하여 남은 작업 길이 (code length)가 가장 짧은 프로세스를 선택합니다.
<br>
RT 프로세스에 의해 preemption 된 normal process 도 동일하게 처리합니다.
## os2-3.c
#### Round-Robin Scheduling.
stdin으로부터 이진파일 형태의 프로세스 정보를 입력받습니다.
<br>
그 프로세스를 Round-Robin(RR)으로 스케줄링 하도록 구현합니다.
<br>
한 라운드가 시작될 때, 정책에 따라 각 프로세스에게 time slice (TS)를 50 clocks 씩 부여합니다.
<br>
라운드 내에서는 각 프로세스들에 대해 FIFO 스케줄링 기법을 사용합니다.
## os2-4.c
#### Evaluation. 스케줄링 성능 분석 및 평가.
저는 과제 2-4를 보다 쉽게 하고자 스케줄링 테스트 프로그램을 제작하였습니다.
<br>
프로그램을 간략히 설명하자면 원하는 프로세스를 입력받아 여러 스케줄링 기법을 시뮬레이트 해주는 프로그램입니다.
<br>
자세한 작동 기전 및 실행 화면은 과제 보고서.pdf에 기재되어있습니다.
## os2-4test2.c
#### test2.bin 의 프로세스로 이루어진 os2-4.c 프로그램
os2-4.c를 이용해서 매번 test2.bin 프로세스를 입력하기 너무 불편했습니다.
<br>
이에, test2.bin과 동일한 프로세스를 이용하여 스케줄링을 바꾸어 해보는 코드도 제작하였습니다.
<br>
과제 os2.c에서는 Real_time 구현이 되있지 않으나, test2.bin에는 Real_time 프로세스가 있으므로 이 코드에서는 구현이 되었습니다.
<br>
위 os2-1~3 코드와 동일한 스케줄링 조건을 선택하게 된다면, 결과 값이 동일하게 나오는 것을 확인 할 수 있습니다.
<br>
이 코드는 과제 보고서.pdf 에서는 간략하게 하고자 기재되지 않았습니다.
## 과제 보고서.pdf
#### 소스코드에 대한 간략한 설명 및 스케줄링 비교 보고서
위의 코드들을 이용하여 작성한 보고서입니다. 코드들의 대략적인 작동기전을 알 수 있고, os2-4.c를 이용해 스케줄링 효율성을 비교해 본 것을 보고서로 작성하였습니다. 
