#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define PAGESIZE (32)

#define PAS_FRAMES (256)  // fit for unsigned char frame in PTE

#define PAS_SIZE (PAGESIZE*PAS_FRAMES) //32*256 = 8192 B

#define VAS_PAGES (64)

#define VAS_SIZE (PAGESIZE*VAS_PAGES)  // 32*62 = 2048 B

#define PTE_SIZE (4)  // sizeof(pte)

#define PAGETABLE_FRAMES (VAS_PAGES*PTE_SIZE/PAGESIZE) //64*4/32 = 8 consecutive frames

#define PAGE_INVALID (0)

#define PAGE_VALID (1)

#define MAX_REFERENCES (256)


typedef struct{

    unsigned char frame; // allocated frame

    unsigned char vflag; // valid-invalid bit 

    unsigned char ref;   // reference bit

    unsigned char pad;   // padding
} pte; // Page Table Entry (total 4 bytes, always)



typedef struct {

    int pid;

    int ref_len;  // Less than 255

    unsigned char *references;
} process_raw;



typedef struct{

    unsigned char b[PAGESIZE];

} frame;


typedef struct{

    process_raw pcess;

    int AllocatedFrames;

    int PageFault;

    int ReferenceCount;

}process;




int main(int argc, char* argv[]) {


    process *cur; // cur : 파일 읽은거 담을 구조체(프로세스) 포인터

    cur = (process *) malloc(sizeof(process));


    process* processArr[10];         // bin 파일에서 받는 프로세스 저장할 배열
    int processnum = 0;           // bin 파일에서 몇개의 프로세스 받았는지 저장

    while(fread(cur, sizeof(int)*2, 1, stdin) == 1) { //프로세스 읽기, 실패시 while문 종료

        processArr[processnum] = (process *) malloc(sizeof(process)); 

        processArr[processnum]->pcess.pid = cur->pcess.pid;
        processArr[processnum]->pcess.ref_len = cur->pcess.ref_len;

		processArr[processnum]->pcess.references = (unsigned char *) malloc(processArr[processnum]->pcess.ref_len); // 할당
        fread(processArr[processnum]->pcess.references,processArr[processnum]->pcess.ref_len,1,stdin); //코드 넣기

        
        processArr[processnum]->AllocatedFrames = 1; // 기본으로 Level 1 Page Table 할당되므로 기본값 1으로 설정
        processArr[processnum]->PageFault =0;
        processArr[processnum]->ReferenceCount =0;


        processnum++;

    }

    free(cur);



    frame* PAS = (frame*)malloc( PAS_SIZE ); // 물리 메모리



    int UsedPAS = 0;               // 물리 메모리에서 사용된 frame수를 저장. PAS[UsedPAS] 는 메모리가 다음에 써져야 할 공간을 의미함
    UsedPAS += processnum * 1;  // 프로세스 갯수만큼 Table 할당됨. 



    pte* cur_pte_L1;  // 테이블 접근 위한 포인터 (level 1)
    pte* cur_pte_L2;  // 테이블 접근 위한 포인터 (level 2)

    int i;
    int j;
    for( i = 0 ; i < processnum ; i++) // page table 초기화
    {
        cur_pte_L1 = (pte *) &PAS[i*1];

        for( j = 0 ; j < 8 ; j++)
        {
            cur_pte_L1[j].vflag = 0; // 0이면 유효하지 않음. 1이면 유효함
            cur_pte_L1[j].ref = 0;   // 초기 ref = 0.
        }

    }




    // 진행 시작
    int REF = 0;  // Reference 진행할 변수
    

    while( REF < 256 && UsedPAS < 256) // 레퍼런스 다 보거나 PAS가 사용 가능할떄
    {
        for( i = 0 ; i < processnum; i++ )
        {
            // 헷갈리지 않게 현재 진행중인 process 변수 갱신.
            cur = processArr[i];               //현재 ref하는 프로세스

            if( cur->pcess.ref_len > REF) // Refer할게 있다면
            {


                cur_pte_L1 = (pte *) &PAS[i*1];       // ref하는 프로세스의 Level 1 page table 갱신


                if( cur_pte_L1[cur->pcess.references[REF]/8].vflag == 0 ) // L1 할당이 되어있지 않다면. 할당. ( L1 Page Fault )
                {
                    if( UsedPAS >= 256 )  // 메모리 부족시 중지
                    {
                        break;
                    }
                    
                    cur->PageFault += 1;
                    cur_pte_L1[cur->pcess.references[REF]/8].frame = UsedPAS;   // 새로운 물리 프레임 할당 저장
                    cur_pte_L1[cur->pcess.references[REF]/8].vflag = 1;         // 유효하다고 표시
                    cur->AllocatedFrames += 1;
                    UsedPAS++;
                }
                



                cur_pte_L2 = (pte *) &PAS[cur_pte_L1[cur->pcess.references[REF]/8].frame];       //Level 2 page table로 cur_pte 갱신.
                

                if( cur_pte_L2[cur->pcess.references[REF]%8].vflag == 1 ) // L2 에서 할당 되어 있음.
                {
                    // 접근
                    cur_pte_L2[cur->pcess.references[REF]%8].ref += 1;
                    cur->ReferenceCount += 1;
                }
                else // L2 Page Fault
                {
                    if( UsedPAS >= 256 )
                    {
                        break;
                    }
                    
                    cur->PageFault += 1;
                    cur->ReferenceCount += 1;  // 프로세스의 ref 카운트 더하기
                    cur_pte_L2[cur->pcess.references[REF]%8].frame = UsedPAS;   // 새로운 물리 프레임 할당 저장
                    cur_pte_L2[cur->pcess.references[REF]%8].ref += 1;          // ref 더하기
                    cur_pte_L2[cur->pcess.references[REF]%8].vflag = 1;         // 유효하다고 표시
                    cur->AllocatedFrames += 1;
                    UsedPAS++;
                }


            }
       
        }
        
        REF++;
    }



    if( UsedPAS >= 256 && REF < 256 ) // 메모리가 부족해서 종료된것인지 확인
    {
        fprintf(stdout, "Out of memory!!\n");
    }

    int k;
    int AllFaults = 0;
    int AllRef = 0;

    // 각 프로세스별 정보 및 페이지 테이블 출력
    for( i = 0 ; i < processnum ; i++)
    {
        fprintf(stdout, "** Process %03d: Allocated Frames=%03d PageFaults/References=%03d/%03d\n" , processArr[i]->pcess.pid, processArr[i]->AllocatedFrames,  processArr[i]->PageFault, processArr[i]->ReferenceCount);
        AllFaults += processArr[i]->PageFault;
        AllRef += processArr[i]->ReferenceCount;

        cur_pte_L1 = (pte *) &PAS[i*1];

        for( j = 0 ; j < 8 ; j++)
        {
            if(cur_pte_L1[j].vflag == 1)
            {
                fprintf(stdout, "(L1PT) %03d -> %03d\n" , j, cur_pte_L1[j].frame);  // L1 출력

                cur_pte_L2 = (pte *) &PAS[cur_pte_L1[j].frame];

                for( k = 0 ; k < 8 ; k++)
                {
                    if(cur_pte_L2[k].vflag == 1)
                    {
                        fprintf(stdout, "(L2PT) %03d -> %03d REF=%03d\n" , j*8+k, cur_pte_L2[k].frame, cur_pte_L2[k].ref); // L2 출력

                    }
                }
            }

        }

    }

    // 모든 작업 종료 후 최종리포트
    fprintf(stdout, "Total: Allocated Frames=%03d Page Faults/References=%03d/%03d\n" , UsedPAS,AllFaults, AllRef);


    for( i = 0 ; i < processnum ; i++) // 할당 해제
    {
        free(processArr[i]->pcess.references);
        free(processArr[i]);
    }
    free(PAS);

}