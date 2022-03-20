#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int pid;            //ID
    int arrival_time;   //도착시간
    int code_bytes;     //코드길이(바이트)
} process;

typedef struct {
    unsigned char movement; // 동작
    unsigned char time;     // 길이(동작 수행 시간)
} tuple;


int main(int argc, char* argv[]) {
    
    process data; //파일 읽은거 담을 구조체(프로세스) 선언

    while(fread(&data, sizeof(process), 1, stdin) == 1) { //프로세스 읽기, 실패시 while문 종료
        fprintf(stdout, "%d %d %d\n", data.pid, data.arrival_time, data.code_bytes); //프로세스 정보 출력

        int i = data.code_bytes/2; //i는 튜플의 갯수. 한개의 튜플이 2바이트 이므로 2로 나누어줌
        int j;

        for( j = 0 ; j < i ; j++ ) // 튜플 갯수만큼 반복.
        {
            tuple temporary; //파일 읽은거 담을 구조체(튜플) 선언

            if(fread(&temporary, sizeof(tuple) , 1, stdin) == 0 )    // 튜플 읽기, 예외처리
            {
                fprintf(stdout, "읽을 항목이 없습니다.\n");
                return 0;
            }

            fprintf(stdout, "%d %d\n", temporary.movement, temporary.time); // 튜플 출력
        }
    }

    return 0;
}