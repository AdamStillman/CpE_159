// tool.c, 159

#include "spede.h"
#include "type.h"
#include "extern.h"

int EmptyQ(q_t *p){
	return (p->size == 0);// try <=0
}

int FullQ(q_t *p){
	return (p->size==MAX_PROC);
}

void initq(q_t *q){
	q->size = q->head = q->tail = 0;
}

void MyBzero(char *p, int size) { //trying changing char to Q_t
   int i;
   for(i=0; i<size; i++){
	*p++ = '\0';
   }
} 

void MyBZero(void *s, int n) {
int i;
for (i = 0; i < n; i++) {
	*((char*)s+n) = '\0';
	}
}

void EnQ(int pid, q_t *p) {
	if(p->size == Q_SIZE ){
		cons_printf("Que is full! in enq\n");	
		return;
	}

	p->q[p->tail] = pid;
	p->tail++;
	if(p->tail == Q_SIZE) p->tail=0;
	p->size++;
}

int DeQ(q_t *p) { // return -1 if q is empty
	int pid;
	if(p->size ==0){
    		printf("Queue is empty in deq\n");
    		return -1;////////try Empty(&p)
   	}

	pid = p->q[p->head];
	p->head++;
	if(p->head == Q_SIZE)p->head=0;
	p->size--;

	return pid;	
}

void MsgEnQ(msg_t *p, msg_q_t *q){
	if(q->size == Q_SIZE){
		cons_printf("MsgEnQ passed full queue");
		return;
	}
	q->msg[q->tail] = *p;
	q->tail++;
	if(q->tail == Q_SIZE) q->tail = 0;
	q->size++;
}

msg_t *MsgDeQ(msg_q_t *p){ // return -1 if q is empty
	msg_t* msg;

	if(p->size == 0) {
		cons_printf("Empty queue passed to MsgDeQ");
		return (msg_t*)0;
	}

	msg = &p->msg[p->head];
	p->head++;
	if(p->head == Q_SIZE)p->head = 0;
	p->size--;

	return msg;	
}

void MyStrCpy(char *dest, char *src){
	/*char *p;
	int a=0;

	for(p=src; p!='\0'; p++){
		*dest=*p;
		a++;
	}
	
	dest[a]='\0';  */
   while (*src ) {
      *dest = *src;
      src++;
      dest++;
   }
   *dest = '\0';
}	

int MyStrCmp(char *s1, char *s2){
	while(*s1 && *s2){
		if(*s1 == *s2){
			s1++;
			s2++;
		}
		else return 0;
		if(*s1 != *s2) return 0;
		
	}
	return 1;
}

int MyStrLen( char *s){
	int count=0;
	while(*s!='\0'){
		count++;
		s++;
	}
	return count;
}

void MyMemcpy(char *dest, char *src, int size){
	int b;
	for(b = 0; b < size ; b++){
		*dest = *src;
		src++;
		dest++;
	}
}

int MyStrcmpSize(char *p, char *q, int size){
	int b;
	for(b = 0; b < size ; b++){
		if(*p == *q){
			p++;
			q++;
		}else 
			return 0;
	}
	return 1;
}
