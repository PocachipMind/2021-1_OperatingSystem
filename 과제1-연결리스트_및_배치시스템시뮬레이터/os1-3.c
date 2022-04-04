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
	int PC;             //PC레지스터
	int ioOpEnd;        //wait_q에서 io명령어 종료시간 저장
    code *operations;   //code tuples 가 저장된 위치
    struct list_head job, ready, wait;
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
		next->ioOpEnd = 0;

		next -> operations = (code *) malloc(next->code_bytes); // 할당
		fread(next->operations,next->code_bytes,1,stdin); //코드 넣기

        INIT_LIST_HEAD(&next->job);
        INIT_LIST_HEAD(&next->ready);
        INIT_LIST_HEAD(&next->wait);   // 초기화

        list_add_tail(&next->job,&job_q); //잡큐에 넣기

    }

	cur->pid = 100;
	cur->arrival_time = 0;
	cur->code_bytes = 2;
	cur->PC = 0;
	cur->ioOpEnd = 0;
	cur->operations = (code *) malloc(cur->code_bytes);
	cur->operations[0].op = 0xFF;
	cur->operations[0].len = 0;
	INIT_LIST_HEAD(&cur->job);
	INIT_LIST_HEAD(&cur->ready);
	INIT_LIST_HEAD(&cur->wait);
	list_add_tail(&cur->job,&job_q);    // Idle 프로세스 초기화 후 잡큐에 추가

	
	int clock = 0;
	int idle_time = 0;
	int jobQnum = 0;        //잡큐에 들어온 프로세스 개수
	int finishNum = 0;      //레디큐에 들어왔다가 종료한 프로세스 개수
	int cpuOpEnd = 0;       //CPU명령어 끝나는 clock시간 저장
	bool isworkCPU = true; //CPU가 일을 했는지
	int runningPID = 0;
	LIST_HEAD(ready_q);     //레디큐 리스트
	LIST_HEAD(wait_q);      //웨이트큐 리스트

	list_for_each_entry(cur, &job_q, job){
		jobQnum++;
	}

	while( jobQnum-1 != finishNum || cpuOpEnd >= clock ) // 잡큐와 레디큐 들어온 프로세스 개수가 동일, 현재 프로세스가 다 끝날때까지,  clock수 늘림
	{

		// 도착하면 레디큐에 넣음.
		list_for_each_entry(cur, &job_q, job){

			if( cur->arrival_time == clock){ //도착시간이 clock과 같다면

				list_add_tail(&cur->ready,&ready_q); //레디큐에 프로세스 추가
				fprintf(stdout, "%04d CPU: Loaded PID: %03d\tArrival: %03d\tCodesize: %03d\tPC: %03d\n", clock, cur->pid, cur->arrival_time, cur->code_bytes, cur->PC); //로드된 프로세스 정보 출력
			}

		}

		// wait_q에서 io명령어 다하면 ready_q로 이동
		list_for_each_entry_safe(cur, next, &wait_q, wait){

			if( cur->ioOpEnd <= clock){ //io 명령어 수행 완료하면
				//wait_q에서 ready_q로 옮김
				list_del_init(&cur->wait);
				list_add_tail(&cur->ready,&ready_q);
				fprintf(stdout, "%04d IO : COMPLETED! PID: %03d\tIOTIME: %03d\tPC: %03d\n", clock, cur->pid, clock, cur->PC); //IO 작업 종료 정보 출력
				cur->PC += 1;
			}

		}



		// 레디큐 맨 앞의 프로세스 작업
		cur = list_entry(ready_q.next, process, ready); // 레디큐 맨 앞꺼.

		if( cur->pid != 100 && clock >= cpuOpEnd  ) //ready큐 맨 앞 프로세스가 idle 프로세스 아닌것. 단, 현재 실행중인 명령어가 끝난경우만
		{ 
			if( cur->PC < cur->code_bytes/2)// 프로세스 모든 명령어가 끝났는 지 확인
			{
				if(runningPID != cur->pid) // 이전까지 수행하던 프로세스 PID가 지금 PID와 다르면 컨텍트 스위치
				{
					clock += 10;
					idle_time += 10;
					fprintf(stdout, "%04d CPU: Switched\tfrom: %03d\tto: %03d\n" , clock, runningPID, cur->pid); //CPU 작업 전환 정보 출력
					runningPID = cur->pid;
					int i;
					for(i=1;i<10;i++)   // 컨텍트 스위치 도중에 Arrival하는 프로세스 추가.
					{
						list_for_each_entry(next, &job_q, job)
						{
							if( next->arrival_time == (clock-10+i)){ // 컨텍트 스위치 도중 Arrival한 프로세스는 끝나고 도달한걸로 표시됨.

								list_add_tail(&next->ready,&ready_q); //레디큐에 프로세스 추가
								fprintf(stdout, "%04d CPU: Loaded PID: %03d\tArrival: %03d\tCodesize: %03d\tPC: %03d\n", clock, next->pid, next->arrival_time, next->code_bytes, next->PC); //로드된 프로세스 정보 출력
								
							}
							
						}

					}
					
				}

				if(cur->operations[cur->PC].op == 0) // cpu작업인경우
				{
					cpuOpEnd = cur->operations[cur->PC].len + clock; //이 명령어가 끝나는 시간 저장
					cur->PC += 1;                                     //PC레지스터 1더해서 명령어 다음꺼 생각해둠
					isworkCPU = true;
				}else if(cur->operations[cur->PC].op == 1) //io 작업인경우
				{
					cur->ioOpEnd = cur->operations[cur->PC].len + clock;
					//ready 큐에서 wait 큐로 옮김
					list_del_init(&cur->ready);
					list_add_tail(&cur->wait,&wait_q);
				}
				
			}else // 이 프로세스 명령어가 다 끝났음
			{
				list_del_init(&cur->ready); // 프로세스 명령어 다 수행 끝났으니 레디큐에서 삭제
				finishNum++; // 종료 개수 1개증가.
				continue; // 프로세스의 명령어가 끝났으니 아무행동도 하지않음 = clock이 소모되지 않음.
			}
			
		}else if( cur->pid == 100 ) // idle 프로세스
		{
			list_del_init(&cur->ready);
			if(!list_empty(&ready_q)) // idle프로세스 이외의 다른 프로세스 존재
			{
				list_add_tail(&cur->ready,&ready_q); // idle 프로세스 ready큐 마지막으로 보냄.
				continue; // idle 프로세스는 우선순위가 낮음. 즉 스케줄링으로 리스트 뒤로 보낸것을 코드로써 표현될 뿐 실제로는 아무 행동도 하지않음
			}
			else if(list_empty(&ready_q)) //ready큐에 idle프로세스만 있음
			{
				if( !list_empty(&wait_q) ) // wait_q에 프로세스 존재
				{   
					list_add_tail(&cur->ready,&ready_q);
										
					if(runningPID != cur->pid) // 컨텍트 스위치
					{
						clock += 10;
						idle_time += 10;
						fprintf(stdout, "%04d CPU: Switched\tfrom: %03d\tto: %03d\n" , clock, runningPID, cur->pid); //CPU 작업 전환 정보 출력
						runningPID = cur->pid; //100이다.
						int i;
						for(i=1;i<10;i++)   // 컨텍트 스위치 도중에 Arrival하는 프로세스 추가.
						{
							list_for_each_entry(next, &job_q, job)
							{
								if( next->arrival_time == (clock-10+i)) // 컨텍트 스위치 도중 Arrival한 프로세스는 끝나고 도달한걸로 표시됨.
								{
									list_add_tail(&next->ready,&ready_q); //레디큐에 프로세스 추가
									fprintf(stdout, "%04d CPU: Loaded PID: %03d\tArrival: %03d\tCodesize: %03d\tPC: %03d\n", clock, next->pid, next->arrival_time, next->code_bytes, next->PC); //로드된 프로세스 정보 출력
								}
							}	
						}	
					}	
						
						
				}
				else if( list_empty(&wait_q) ) // wait_q에 프로세스가 없다면
				{
					if(jobQnum-1 == finishNum) // job_q속 idle 제외 프로세스 개수와 끝낸 프로세스 개수 동일
					{
						finishNum++;
						break;  //모든 프로세스를 다했음. clock소모 없이 그냥 끝내면됨.			
					}
					else if (jobQnum-1 != finishNum) // 아직 도착 안한 프로세스 존재
					{
						list_add_tail(&cur->ready,&ready_q);

						if(runningPID != cur->pid) // 컨텍트 스위치
					    {
						    clock += 10;
					    	idle_time += 10;
							fprintf(stdout, "%04d CPU: Switched\tfrom: %03d\tto: %03d\n" , clock, runningPID, cur->pid); //CPU 작업 전환 정보 출력
						    runningPID = cur->pid; //100이다.
						    int i;
						    for(i=1;i<10;i++)   // 컨텍트 스위치 도중에 Arrival하는 프로세스 추가.
					    	{
						    	list_for_each_entry(next, &job_q, job)
						    	{
							    	if( next->arrival_time == (clock-10+i)) // 컨텍트 스위치 도중 Arrival한 프로세스는 끝나고 도달한걸로 표시됨.
							    	{
								    	list_add_tail(&next->ready,&ready_q); //레디큐에 프로세스 추가
								    	fprintf(stdout, "%04d CPU: Loaded PID: %03d\tArrival: %03d\tCodesize: %03d\tPC: %03d\n", clock, next->pid, next->arrival_time, next->code_bytes, next->PC); //로드된 프로세스 정보 출력
								    }
							    }	
						    }	
					    }

					}
				}
			}

		}


		if(runningPID == 100)
		{
			isworkCPU = false;
		}

		if(!isworkCPU) // cpu 일 안했으면 idle시간 증가
		{
			idle_time++;
		}
		clock++;

	}

	//최종 리포트
	fprintf(stdout, "*** TOTAL CLOCKS: %04d IDLE: %04d UTIL: %2.2f%%\n", clock, idle_time, (double)(clock-idle_time)*100/clock);

	//fprintf(stdout, "finish = %d \n", finishNum);
	

	// 리스트 제거, 할당 해제 : 이미 선언한 cur과 next 사용 
	list_for_each_entry_safe(cur, next, &job_q, job){
		list_del(&cur->job);
		free(cur->operations);
		free(cur);
	}

    return 0;
}