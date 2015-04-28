// isr.c, 159
//changes made and working
#include "spede.h"
#include "type.h"
#include "isr.h"
#include "tool.h"
#include "extern.h"
#include "proc.h"
#include "syscall.h"
#include "FileMgr.h"

int wakingID;//why isnt this working file mgr wtf

void CreateISR(int pid) {
//cons_printf("in create ISR\n");
	if(pid!=0) {
	pcb[pid].mode = UMODE;   //mode should be set to UMODE
	pcb[pid].state = RUN;   //state should be set to RUN
	pcb[pid].runtime = pcb[pid].total_runtime = 0;   //both runtime counts should be set 0
	EnQ(pid, &run_q);  // if pid is not for Idle (0), enqueue pid to run queue
	}
	
	//added code
	MyBZero(stack[pid], STACK_SIZE); //erase stack
	//phase5
	MyBZero( &mbox[pid], sizeof(mbox_t)); //clear the mbox

	
	pcb[pid].TF_ptr = (TF_t *)&stack[pid][STACK_SIZE];
	pcb[pid].TF_ptr--;
	//point above the stack then drop by sizeof(TF_t)
	if(pid == 0){
		pcb[pid].TF_ptr->eip = (unsigned int)Idle; // Idle process
	//	cons_printf("in idle\n");
	}
	else if(pid==1){
		pcb[pid].TF_ptr->eip = (unsigned int) Init;
	}
	else if(pid==2){
		pcb[pid].TF_ptr->eip = (unsigned int) PrintDriver;
	}else if(pid == 3){
		pcb[pid].TF_ptr->eip = (unsigned int)Shell;
	}else if(pid == 4){
		pcb[pid].TF_ptr->eip = (unsigned int)STDIN;
	}else if(pid == 5){
		pcb[pid].TF_ptr->eip = (unsigned int)STDOUT;
	}
	else if(pid == 6){
		pcb[pid].TF_ptr->eip = (unsigned int)FileMgr;
	}
	else pcb[pid].TF_ptr->eip = (unsigned int) UserProc;

		//fillout trapframe of new proc
   	pcb[pid].TF_ptr->eflags = EF_DEFAULT_VALUE | EF_INTR;
 	pcb[pid].TF_ptr->cs = get_cs();
   	pcb[pid].TF_ptr->ds = get_ds();
   	pcb[pid].TF_ptr->es = get_es();
   	pcb[pid].TF_ptr->fs = get_fs();
   	pcb[pid].TF_ptr->gs = get_gs();
	
}

void TerminateISR() {

	if(CRP == 0 || CRP == -1) return;   //just return if CRP is 0 or -1 (Idle or not given)     
	else{
	pcb[CRP].state = NONE;   //change the state of CRP to NONE
	EnQ(CRP, &none_q);   //and queue it to none queue
	CRP = -1;   //set CRP to none (-1)
	}
}        

void TimerISR() {
   int size, pid;

   outportb(0x20, 0x60); 
   sys_time++;

   size = sleep_q.size;
   while(size--) {
      pid = DeQ(&sleep_q);
      if(pcb[pid].wake_time > sys_time)
         EnQ(pid, &sleep_q);
      else {
         EnQ(pid, &run_q);
         pcb[pid].state = RUN;
      }
   }

   if(CRP <= 0) return;   //just return if CRP is Idle (0) or less (-1)

   pcb[CRP].runtime++;   //upcount the runtime of CRP
   if(pcb[CRP].runtime == TIME_LIMIT){   //if the runtime reaches the set time limit
		pcb[CRP].total_runtime += TIME_LIMIT;      //simply total up the total runtime of CRP
		 pcb[CRP].runtime = 0;   //(need to rotate to the next in run queue)
		 pcb[CRP].state = RUN;
		 EnQ(CRP, &run_q);      //queue it to run queue
		 CRP = -1;     // reset CRP (to -1, means none)
	}
}

void SleepISR(int time_sleep) {
	pcb[CRP].wake_time = sys_time + time_sleep * 100;
	EnQ(CRP, &sleep_q);
	pcb[CRP].state = SLEEP;
	CRP = -1;
}

void GetPidISR(){
	pcb[CRP].TF_ptr->ebx = CRP;
	//EnQ(CRP, &run_q);
	//pcb[CRP].state = RUN;
	//GetPid();
}

void SemWaitISR(){
  //if semaphore queue is greater than 0 it will decrement the count
  int semaphoreID = pcb[CRP].TF_ptr->ebx;
  if(semaphore[semaphoreID].count > 0){
   semaphore[semaphoreID].count--;
  }
  //if semaphore queue is equal to 0 the current running process will be queued
  else {
    EnQ(CRP, &(semaphore[semaphoreID].wait_q));
    pcb[CRP].state = WAIT;
    CRP=-1;
  }
}

void SemPostISR(int semaphoreID){
   if(semaphore[semaphoreID].wait_q.size == 0){
     semaphore[semaphoreID].count++;
   } else {
     int temp = DeQ(&(semaphore[semaphoreID].wait_q));
     EnQ(temp, &run_q);				
     pcb[temp].state = RUN;     
   }
}

void SemGetISR(){
	//count in ebx
	int count = pcb[CRP].TF_ptr->ebx;
	int sid = DeQ(&semaphore_q);//semaphore_q

	if(sid != -1){
	   MyBzero((char *)&semaphore[sid], sizeof(semaphore_t));
	   semaphore[sid].count = count;
	}
	pcb[CRP].TF_ptr->ecx = sid;
}

void IRQ7ISR(){
//int pid;
   int temp;
   outportb(0x20, 0x67);
//pid = pcb[CRP].TF_ptr->ebx;
   if(semaphore[print_semaphore].wait_q.size == 0)
      semaphore[print_semaphore].count++;
   else {
	temp = DeQ(&(semaphore[print_semaphore].wait_q));
	EnQ(temp, &run_q);
	pcb[temp].state = RUN;
   }
}

void MsgSndISR(){
	msg_t *source, *destination;
	int msg_id;
	source = (msg_t *)pcb[CRP].TF_ptr->ebx;
	msg_id = source -> recipient;
	
	source->sender = CRP;
	source->time_stamp = sys_time;

	if( mbox[msg_id].wait_q.size == 0 ){
		MsgEnQ(source, &mbox[msg_id].msg_q);
	}
	else{
		int temp_pid = DeQ(&(mbox[msg_id].wait_q));
		EnQ(temp_pid, &run_q);
		pcb[temp_pid].state = RUN;
		destination = (msg_t *)pcb[temp_pid].TF_ptr->ebx;
		memcpy((char*)destination,(char*)source,sizeof(msg_t));
	}
}

void MsgRcvISR(){
	msg_t *source, *destination;	

	if( (mbox[CRP].msg_q).size == 0){
		EnQ(CRP, &(mbox[CRP].wait_q));//clock crp
		pcb[CRP].state = WAIT;
		CRP = -1;
	}
	else{
		source = MsgDeQ(&mbox[CRP].msg_q);
		destination = (msg_t *)pcb[CRP].TF_ptr->ebx;
		memcpy((char *)destination,(char *)source,sizeof(msg_t));
	}
}

void IRQ3TX(){
	char ch = '\0';
	if(!EmptyQ(&terminal.echo_q)) ch = DeQ(&terminal.echo_q);
	else{
		if(!EmptyQ(&terminal.TX_q)){
			ch = DeQ(&terminal.TX_q);
			SemPostISR(terminal.TX_sem);
		}
	}
	if(ch == '\0') terminal.TX_extra = 1;
	else{
		outportb(COM2_IOBASE+DATA,ch);
		terminal.TX_extra = 0;
	}
}
void IRQ3RX(){
	char ch;
	ch = inportb(COM2_IOBASE+DATA) & 0x7F;
	EnQ(ch, &terminal.RX_q);
	SemPostISR(terminal.RX_sem);
	if(ch == '\r'){
		EnQ((int)'\r', &terminal.echo_q);
		EnQ((int)'\n', &terminal.echo_q);
	}
	else{
	if(terminal.echo==1) EnQ((int)ch, &terminal.echo_q);
}
}

void IRQ3ISR(){//phase6
	int event;
	outportb(0x20, 0x63); //dismiss IRQ 3: use outportb() to send 0x63 to 0x20
	//read event from COM2_IOBASE+IIR (Interrupt Indicator Register
	event = inportb(COM2_IOBASE+IIR);
	switch(event) {
		case IIR_TXRDY:// (send char to terminal video)
		IRQ3TX();
		break;
		case IIR_RXRDY://(get char from terminal KB)
		IRQ3RX();
		break;
	}
	if(terminal.TX_extra==1) IRQ3TX();
}


