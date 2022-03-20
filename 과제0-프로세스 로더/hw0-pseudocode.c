#include <stdio.h>
#include <stdlib.h>

typedef struct code_t {
    unsigned char op;
    unsigned char len;    
} code;

typedef struct {
    int pid;            //ID
    int arrival_time;   //도착시간 
    int code_bytes;     //코드길이(바이트)    
} process;

int main(int argc, char* argv[]) {    
    process *cur;   
    code *cur_code;

    //cur, cur_code를 동적 할당하여 활용

    while() { : fread(cur,sizeof(process),1,stdin) 가 성공하면 계속 반복
        
        //로드한 프로세스 정보 출력  

        for() { : cur->code_bytes/2 만큼 반복
            fread(&cur_code->op,sizeof(unsigned char),1,stdin);
            fread(&cur_code->len,sizeof(unsigned char),1,stdin);
            
            //로드한 코드 정보 출력
        }
    }

    //동적 할당한 메모리 해제

    return 0;
}