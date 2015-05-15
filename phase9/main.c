// main.c, 159
// simulated kernel
//
// Team Name: TSLK (Members: SEAN KENNEDY and ADAM STILLMAN)

#include "spede.h"      // spede stuff
#include "main.h"       // main stuff
#include "isr.h"        // ISR's     
#include "tool.h"       // handy functions
#include "proc.h"       // processes such as Init()
#include "type.h"       // processes such as Init()
#include "entry.h"

// kernel data structure:
int CRP, sys_time;                // current running PID, -1 means no process
q_t run_q, none_q, sleep_q;      // processes ready to run and not used
pcb_t pcb[MAX_PROC];    // process table

char stack[MAX_PROC][STACK_SIZE]; // run-time stacks for processes
struct i386_gate *IDT_ptr;

semaphore_t semaphore[MAX_PROC];
int print_semaphore;
//int product; //phase3 to be removed
//int product_semaphore;//phase3 to be removed
q_t semaphore_q;

//phase5
mbox_t mbox[MAX_PROC];

//phase 6
terminal_t terminal;

//phase 8
page_t page[MAX_PROC];

//phase 9
int sys_main_table;

void SetEntry(int entry_num, func_ptr_t func_ptr) {
	struct i386_gate *gateptr = &IDT_ptr[entry_num];
	//cons_printf("in set enty\n");	
	fill_gate(gateptr, (int)func_ptr, get_cs(), ACC_INTR_GATE,0);
	//cons_printf("laeving set entry\n");
}

int main() {
	InitData(); // to initialize kernel data
	InitIDT();
	CreateISR(0); //to create Idle process (PID 0)
	//cons_printf("pcb[0] is at %u. \n", pcb[0].TF_ptr);
	Dispatch(pcb[0].TF_ptr);

   return 0;
}




void InitData() {
	int i, j, pid;
        sys_time = 0;
	CRP = 0;
    //initialize queues (use MyBzero() call)
//	initq(&run_q);
//	initq(&none_q);
//	initq(&sleep_q);
	MyBzero((char *) &run_q, sizeof(q_t) );
	MyBzero((char *) &none_q, sizeof(q_t));
	MyBzero((char *) &sleep_q, sizeof(q_t));
	MyBzero((char *) &semaphore_q, sizeof(q_t)); //clears the semaphore queue
	
	//set  to 0
 	for(i=1; i<Q_SIZE; i++){//thats correct
		pcb[i].state = NONE; //set state to NONE in pcb[1~19]
		EnQ(i, &none_q );// queue PID's 1~19 (skip 0) into none_q (not used PID's)	 
		EnQ(i, &semaphore_q );
	}
	for(j=1; j<MAX_PROC; j++ ){
		page[j].owner=-1;
		page[j].addr=0xE00000 + (0x1000*j);
		}
	
	pid = DeQ(&none_q);
	CreateISR(pid);
	pid = DeQ(&none_q);
	CreateISR(pid);
	pid = DeQ(&none_q);
	CreateISR(pid);
	pid = DeQ(&none_q);
	CreateISR(pid);
	pid = DeQ(&none_q);
	CreateISR(pid);
	pid = DeQ(&none_q);
	CreateISR(pid);
	
	sys_main_table = get_cr3();
	
}
//new code
void InitIDT(){
	IDT_ptr = get_idt_base(); 	//get where idt is
	cons_printf("in idt, IDT is at memory location %u. \n", IDT_ptr);
	SetEntry(32, TimerEntry);	//prime IDT entry
	SetEntry(48 , GetPidEntry);
	SetEntry(49 , SleepEntry);
  //new phase
  	SetEntry(50, SemWaitEntry);
  	SetEntry(51, SemPostEntry);
  	SetEntry(52, SemGetEntry);
  	SetEntry(39, IRQ7Entry);
  	SetEntry(53, MsgSndEntry);
  	SetEntry(54, MsgRcvEntry);
  	SetEntry(35, IRQ3Entry);
  	//phase8
  	SetEntry(55, ForkEntry);
  	SetEntry(56, WaitEntry);
  	SetEntry(57, ExitEntry);
	outportb(0x21,~(128+8+1));
}


void SelectCRP() {       // pick PID as CRP
//cons_printf("in selectCRP\n");			//   simply return if CRP is greater than 0
				//   (continue only when CRP is Idle (0) or none (-1)
	if(CRP > 0) return; 

				//   if it's Idle, change its state to RUN
	if(CRP ==0) pcb[0].state = RUN;

				//   if no processes to run (check size in run queue, is it zero)
				//      set CRP to Idle process ID
	if(run_q.size == 0){
      CRP = 0; 

				//   else set CRP to first in run queue (dequeue it)
    } else{ 
	CRP = run_q.q[run_q.head];
    	DeQ(&run_q);
	
}			//   change mode of CRP to UMODE
	    pcb[CRP].mode= UMODE;
//	cons_printf("changed mode to umode\n");
				//   change state of CRP to RUNNING
	    pcb[CRP].state= RUNNING;
//	cons_printf("setting state to rnunning\n");
//cons_printf("leaving crp\n");
    
}

void Kernel(TF_t *TF_ptr) {
//   change state of CRP to kernel mode
	pcb[CRP].mode = KMODE;

//save tf_t to pcb[crp].tf_t
	pcb[CRP].TF_ptr = TF_ptr;
	
//   call timer ISR to service timer interrupt (as if it just occurred)
	switch(TF_ptr->intr_num){
    case TIMER_INTR: 
	TimerISR(); 
	break;
    case SLEEP_INTR:
	SleepISR(TF_ptr->ebx);
	break;
    case GETPID_INTR:
	GetPidISR();		
	break;
    case SEMWAIT_INTR:
      SemWaitISR();
      break;
    case SEMPOST_INTR:
      SemPostISR(TF_ptr->ebx);
      break;
   //phase4
    case SEMGET_INTR:
      SemGetISR(); break;
    case IRQ7_INTR:
      IRQ7ISR(); break;
   //phase5
    case MSGSND_INTR:
      MsgSndISR(); break;
    case MSGRCV_INTR:
      MsgRcvISR(); break;
      //phase6
    case IRQ3_INTR:
      IRQ3ISR(); break;
      //phase8
    case FORK_INTR:
      ForkISR(); break;
    case WAIT_INTR:
      WaitISR(); break;
    case EXIT_INTR:
      ExitISR(); break;
    default: 
	printf("dont PANIC!!!!!!!");
	breakpoint();
	break;
   }
   SelectCRP();
   Dispatch(pcb[CRP].TF_ptr);
}

