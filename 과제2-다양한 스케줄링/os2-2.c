#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#define LIST_POISON1  ((void *) 0x00100100)
#define LIST_POISON2  ((void *) 0x00200200)

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr) do { \
	(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

static inline void __list_add(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next) {
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head) {
	__list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head) {
	__list_add(new, head->prev, head);
}

static inline void __list_del(struct list_head * prev, struct list_head * next) {
	next->prev = prev;
	prev->next = next;
}

static inline void list_del(struct list_head *entry) {
	__list_del(entry->prev, entry->next);
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
}

static inline void list_del_init(struct list_head *entry) {
	__list_del(entry->prev, entry->next);
	INIT_LIST_HEAD(entry);
}

static inline void list_move(struct list_head *list, struct list_head *head) {
        __list_del(list->prev, list->next);
        list_add(list, head);
}

static inline void list_move_tail(struct list_head *list,
				  struct list_head *head) {
        __list_del(list->prev, list->next);
        list_add_tail(list, head);
}

static inline int list_empty(const struct list_head *head) {
	return head->next == head;
}

#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define list_for_each(pos, head) \
  for (pos = (head)->next; pos != (head);	\
       pos = pos->next)

#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; prefetch(pos->prev), pos != (head); \
        	pos = pos->prev)

#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

#define list_for_each_entry(pos, head, member)				\
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = list_entry(pos->member.prev, typeof(*pos), member))

#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		n = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))

#define list_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = list_entry((head)->prev, typeof(*pos), member),	\
		n = list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.prev, typeof(*n), member))

#if 0    //DEBUG
#define debug(fmt, args...) fprintf(stderr, fmt, ##args)
#else
#define debug(fmt, args...)
#endif


typedef struct code_t{
    unsigned char op;  // 동작
    unsigned char len; // 길이(동작 수행 시간)
} code;

typedef struct {
    int pid;            //ID
    int arrival_time;   //도착시간
    int code_bytes;     //코드길이(바이트)
    int PC;
	int Waiting;
	int Response;
	bool isFirst;
    code *operations;   //code tuples 가 저장된 위치
    struct list_head job, Real_Time, Normal, Idle;
} process;


int main(int argc, char* argv[]) {

    process *cur, *next; // cur : 파일 읽은거 담을 구조체(프로세스) 포인터, next : 리스트속에 넣을 실제 구조체로 파일담은 cur에서 옮겨 담을 포인터

    LIST_HEAD(job_q); //잡 큐 리스트

    cur = (process *) malloc(sizeof(process));


    while(fread(cur, sizeof(int)*3, 1, stdin) == 1) { //프로세스 읽기, 실패시 while문 종료
        
		next = (process *) malloc(sizeof(process));

		next->pid = cur->pid;
		next->arrival_time = cur->arrival_time;
		next->code_bytes = cur->code_bytes;      //옮기기
        next->PC = 0;
		next->Waiting =0;
		next->Response =0;
		next->isFirst =true;

		next -> operations = (code *) malloc(next->code_bytes); // 할당
		fread(next->operations,next->code_bytes,1,stdin); //코드 넣기

        INIT_LIST_HEAD(&next->job);
        INIT_LIST_HEAD(&next->Real_Time);
        INIT_LIST_HEAD(&next->Normal);
        INIT_LIST_HEAD(&next->Idle);   // 초기화

        list_add_tail(&next->job,&job_q); //잡큐에 넣기

    }

	cur->pid = 100;
	cur->arrival_time = 0;
	cur->code_bytes = 2;
    cur->PC = 0;
	cur->Waiting=0;
	cur->Response=0;
	cur->isFirst =true;
	cur->operations = (code *) malloc(cur->code_bytes);
	cur->operations[0].op = 0xFF;
	cur->operations[0].len = 0;
	INIT_LIST_HEAD(&cur->job);
	INIT_LIST_HEAD(&cur->Real_Time);
	INIT_LIST_HEAD(&cur->Normal);
    INIT_LIST_HEAD(&cur->Idle);
	list_add_tail(&cur->job,&job_q);    // Idle 프로세스 초기화 후 잡큐에 추가

	
	int clock = 0;
	int idle_time = 0;
	int jobQnum = 0;      //잡큐에 들어온 프로세스 개수
	int FinishProcess = 0;    //완전히 종료한 프로세스 개수
	int endoptime = 0;       //명령어 끝나는 clock시간 저장
    bool isworkCPU = true;
	process *last;            //running PID 대신 쓰이는 포인터
	LIST_HEAD(Real_Time_q);   //리얼타임 큐 리스트
    LIST_HEAD(Normal_q);      //노말 큐 리스트
    LIST_HEAD(Idle_q);        //아이들 큐 리스트

	list_for_each_entry(cur, &job_q, job){
		jobQnum++;
	}

	while( jobQnum-1 != FinishProcess ) // 잡큐속 프로세스수와 명령어 다 끝난 프로세스 들어온 프로세스 개수가 동일할 때까지
	{

		// 도착하면 PID에 따라서 각자 맞는 큐에 넣어짐.
		list_for_each_entry(next, &job_q, job){

			if( next->arrival_time == clock) //도착시간이 clock과 같다면
            { 
                if(next->pid == 100) // Pid가 100이라면
                {
                    list_add_tail(&next->Idle,&Idle_q); //Idle큐에 프로세스 추가   
            	}
                else if(next->pid >= 80) // Pid가 100이 아니고 80 이상이라면
                {
                    list_add_tail(&next->Real_Time,&Real_Time_q); //Real Time 큐에 프로세스 추가  
                }
                else // 나머지
                {
                    list_add_tail(&next->Normal,&Normal_q); //Normal Time 큐에 프로세스 추가  
                }
				fprintf(stdout, "%04d CPU: Loaded PID: %03d\tArrival: %03d\tCodesize: %03d\tPC: %03d\n", clock, next->pid, next->arrival_time, next->code_bytes, next->PC); //로드된 프로세스 정보 출력
			}

		}

		last = cur; // 큐에서 새로운 프로세스 꺼내기전, 미리 실행하던 프로세스 저장하기 위한 포인터
		


		//스케줄링
        if(list_empty(&Real_Time_q)) // Real_Time 큐가 비어있다면.
        {
            if(list_empty(&Normal_q)) // Real_Time 큐가 비어있고 Normal 큐도 비어있다면.
            {
                cur = list_entry(Idle_q.next, process, Idle); // Idle 프로세스 가져오기
            }
            else  // Real_Time 큐가 비어있으나 Normal 큐 프로세스 있다면.
            {
				if( clock >= endoptime ) // 수행중인 명령어가 있다면 Normal 큐에서 스케줄링 하지 않음.
				{
					next = list_entry(Normal_q.next, process, Normal); 
					list_for_each_entry(cur, &Normal_q, Normal)
					{
						if(cur->operations[0].len < next->operations[0].len)
						{
							next = cur;
						}
					}
					cur = next; // Normal 큐에서 명령어길이 가장 작은 프로세스 선택
				}
            }
        }
        else
        {
            cur = list_entry(Real_Time_q.next, process, Real_Time); // Real_Time 큐에서 맨 앞 프로세스 가져오기
        }

		if(clock == 0)  // 처음에는 컨텍트 스위치 없음.
		{
			last = cur;
		}


		//Context Switching
        if( cur->pid != last->pid ) // 이전 실행했던 pid랑 다를경우 컨텍트 스위치
		{
			if(clock < endoptime) // 명령어가 안끝났는데, last 와 cur 프로세스가 다르다면 우선 순위로 인한 프로세스 변경이므로 Preemption.
			{
				last->PC -= 1;
				last->operations[cur->PC].len = (unsigned char)(endoptime - clock); // 수행한 clock 만큼 명령어 길이 감소
				endoptime = clock;
				last->isFirst = false;
			}

			list_for_each_entry(next, &Normal_q, Normal) // 프로세스가 처음이 아닌경우 context swiching은 waiting 시간이므로
			{
				if(!next->isFirst) //큐속 프로세스의 isFirst가 false인 것은 대기중인것임.
				{
					next->Waiting += 5;
				}
			}
			list_for_each_entry(next, &Real_Time_q, Real_Time)
			{
				if(!next->isFirst) //context switching 시간 동안 waiting 시간 더하기
				{
					next->Waiting += 5;
				}
			}

			clock += 5;
			idle_time += 5;
            fprintf(stdout, "%04d CPU: Switched\tfrom: %03d\tto: %03d\n" , clock, last->pid, cur->pid); //CPU 작업 전환 정보 출력
			int i;
			for(i=1 ; i<5 ; i++)   // 컨텍트 스위치 도중에 Arrival하는 프로세스 추가.
			{
				list_for_each_entry(next, &job_q, job)
				{
					if( next->arrival_time == (clock-5+i))// 컨텍트 스위치 도중 Arrival한 프로세스는 끝나고 도달한걸로 표시됨.
                    { 
                        if(next->pid == 100) // Pid가 100이라면
                        {
                            list_add_tail(&next->Idle,&Idle_q); //Idle큐에 프로세스 추가   
                        }
                        else if(next->pid >= 80) // Pid가 100이 아니고 80 이상이라면
                        {
                            list_add_tail(&next->Real_Time,&Real_Time_q); //Real Time 큐에 프로세스 추가  
                        }
                        else // 나머지
                        {
                            list_add_tail(&next->Normal,&Normal_q); //Normal Time 큐에 프로세스 추가  
                        }
                        fprintf(stdout, "%04d CPU: Loaded PID: %03d\tArrival: %03d\tCodesize: %03d\tPC: %03d\n", clock, next->pid, next->arrival_time, next->code_bytes, next->PC); //로드된 프로세스 정보 출력
					}
					
				}

			}
			continue; // 컨텍트 스위치하고 cur을 다시 갱신 해야함.			
		}



		// 실질적 명령어 실행
        if( cur->pid == 100 )
        {
            isworkCPU = false; 
        }
        else if ( clock >= endoptime ) // idle 프로세스(PID = 100) 아니며 현재 진행 중인 명령어가 끝난 경우
        {
            isworkCPU = true; 
            if( cur->PC < cur->code_bytes/2 )// 프로세스의 모든 명령어가 끝났는 지 확인 (안끝남)
            {
				if(cur->isFirst) // 이 프로세스 명령어 처음 실행 
				{
					cur->Response = clock - cur->arrival_time; //클락이 도착한 후 몇 클락 지난지 Response 와 Waiting 에 저장
					cur->Waiting  = clock - cur->arrival_time; 
				}
				endoptime = cur->operations[cur->PC].len + clock; //이 명령어가 끝나는 시간 저장
                cur->PC += 1;                                        //PC레지스터 1더해서 프로세스 끝난것을 표시
            }
            else // 프로세스의 모든 명령어가 끝났음
            {
                if( cur->pid >= 80 ) // realtime 프로세스
                {
                    list_del_init(&cur->Real_Time); // 프로세스 명령어 다 수행 끝났으니 큐에서 삭제
                }
                else // 노말 프로세스
                {
                    list_del_init(&cur->Normal); // 프로세스 명령어 다 수행 끝났으니 큐에서 삭제
                }
                fprintf(stdout, "%04d CPU: Process is terminated\tPID:%03d\tPC:%03d\n" , clock, cur->pid , cur->PC ); // 종료 출력
				FinishProcess++; // 종료 개수 1개증가.
				continue; // 프로세스의 명령어가 끝났으니 아무행동도 하지않음 = clock이 소모되지 않음.
            }
               
        }



		//큐에 존재중인 프로세스 웨이트 타임 증가
		list_for_each_entry(next, &Normal_q, Normal)
		{
			if(!next->isFirst && next->PC < next->code_bytes/2) //isFirst가 false 이며 명령어가 끝나지 않았을때 -> isFirst가 false라면 다른 프로세스로 인해 대기중인 것임.
			{
				next->Waiting += 1;
			}			
		}
		list_for_each_entry(next, &Real_Time_q, Real_Time)
		{
			if(!next->isFirst && next->PC < next->code_bytes/2) //isFirst가 false 이며 명령어가 끝나지 않았을때
			{
				next->Waiting += 1;
			}
		}


		// 클락 관련
		if(!isworkCPU) // cpu 일 안했으면 idle시간 증가
		{
			idle_time++;
		}
		clock++;

	}

	int All_Waiting =0;
	int All_Response =0;

	list_for_each_entry(cur, &job_q, job) // PID 큰거부터 순서대로 출력하기 위해 비어있는 노말큐를 사용.
	{
		list_add_tail(&cur->Normal,&Normal_q); //Job의 프로세스를 전부 노말큐에 넣음.
	}
	
	// 각 프로세스별 정보 출력 : 이미 선언한 cur과 last 사용
	while(!list_empty(&Normal_q)) //노말큐 빌때까지
	{
		last = list_entry(Normal_q.next, process, Normal); 
		list_for_each_entry(cur, &Normal_q, Normal)
		{
			if(cur->pid > last->pid)
			{
				last = cur;
			}
		}
		fprintf(stdout, "PID: %03d\tARRIVAL: %03d\tCODESIZE: %03d\tWAITING: %03d\tRESPONSE: %03d\n", last->pid, last->arrival_time, last->code_bytes, last->Waiting, last->Response);
		All_Waiting += last->Waiting;
		All_Response += last->Response;
		list_del_init(&last->Normal);
	}

	//최종 리포트
	fprintf(stdout, "*** TOTAL CLOCKS: %04d IDLE: %04d UTIL: %2.2f%% WAIT: %2.2f RESPONSE: %2.2f\n", clock, idle_time, (double)(clock-idle_time)*100/clock , (double)All_Waiting/FinishProcess, (double)All_Response/FinishProcess);

	// 리스트 제거, 할당 해제 : 이미 선언한 cur과 next 사용 
	list_for_each_entry_safe(cur, next, &job_q, job){
		list_del(&cur->job);
		free(cur->operations);
		free(cur);
	}
	

    return 0;
}