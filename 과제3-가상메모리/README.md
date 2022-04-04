# 과제 3
3-1. Demand Paging
<br>
3-2. Demand Paging with 2-level Hierarchical Page Table
<br>
과제3 설명.pdf를 읽으시면 조금 더 자세한 과제설명을 알 수 있습니다.
## test3.bin
#### 예시 프로세스 바이너리 파일.
보고서에선 이 바이너리 파일을 기준으로 코드를 실행합니다.
<br>
이 파일에는 2개의 프로세스 정보가 저장되어있습니다.
<br>
<br>
0 번 프로세스 : PID=0, Ref_len=8 
<br>
1 번 프로세스 : PID=1, Ref_len=7
<br>
자세한 파일 정보는 과제3 설명.pdf에 나와있습니다.
## 201619460-3-1.c
#### Demand Paging.
stdin으로부터 이진파일 형태의 프로세스 정보와 Page reference sequence를 입력받습니다.
<br>
그 파일을 통해 Demand Paging을 시뮬레이션 합니다.
## 201619460-3-2.c
#### Demand Paging with 2-level Hierarchical Page Table.
stdin으로부터 이진파일 형태의 프로세스 정보와 Page reference sequence를 입력받습니다.
<br>
그 파일을 통해 Demand Paging with 2-level Hierarchical Page Table을 시뮬레이션 합니다.
<br>
이 코드는 201619460-3-1.c과 작동 기전이 비슷하지만, 2개의 페이지 테이블을 갖습니다. ( Hierarchical Page Table )
## 코드설명보고서.pdf
#### 소스코드에 대한 간략한 설명 및 스케줄링 비교 보고서
위의 코드들을 이용하여 작성한 보고서입니다. 코드들의 대략적인 작동기전을 알 수 있습니다.
