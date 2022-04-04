#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>

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

		next -> operations = (code *) malloc(next->code_bytes); // 할당
		fread(next->operations,next->code_bytes,1,stdin); //코드 넣기

        INIT_LIST_HEAD(&next->job);
        INIT_LIST_HEAD(&next->ready);
        INIT_LIST_HEAD(&next->wait);   // 초기화

        list_add_tail(&next->job,&job_q); //잡큐에 넣기

    }

	free(cur); // 파일 담기, 파일 끝 파악하기 용 동적할당 영역 해제 



	// 리스트 역 순회 : 이미 선언한 cur 사용
	list_for_each_entry_reverse(cur, &job_q, job){
		fprintf(stdout, "PID: %03d\tARRIVAL: %03d\tCODESIZE: %03d\n", cur->pid, cur->arrival_time, cur->code_bytes); //프로세스 정보 출력

		int i = cur->code_bytes/2; //i는 튜플의 갯수. 한개의 튜플이 2바이트 이므로 2로 나누어줌
        int j;
        for( j = 0 ; j < i ; j++ ) // 튜플 갯수만큼 반복.
        {
            fprintf(stdout, "%d %d\n", cur->operations[j].op, cur->operations[j].len); // 튜플 출력
        }
		

	}

	// 리스트 제거, 할당 해제 : 이미 선언한 cur과 next 사용 
	list_for_each_entry_safe(cur, next, &job_q, job){
		list_del(&cur->job);
		free(cur->operations);
		free(cur);
	}

    return 0;
}