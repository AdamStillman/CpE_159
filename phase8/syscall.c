#include "syscall.h" // prototype these below
int GetPid() {
  int pid;
	asm("int $48; movl %%ebx, %0" // CPU inst
		: "=g" (pid) // 1 output from asm()
		: // no input into asm()
		: "%ebx"); // push/pop before/after asm()
  return pid;
}
void Sleep(int sec) {
	asm("movl %0, %%ebx; int $49"
		:
		:"g" (sec)
		:"%ebx");
}

void SemWait (int semaphore_id) {
   asm("movl %0, %%ebx; int $50"
     :
     : "g" (semaphore_id)
     : "%ebx");
}

void SemPost(int semaphore_id) {
    asm("movl %0, %%ebx; int $51"
      :
      : "g" (semaphore_id)
      :"%ebx");
}
//phase 4

int SemGet(int count) {
  int sid;
	asm("movl %1, %%ebx; int $52; movl %%ecx, %0;" // CPU inst
		: "=g" (sid) // 1 output from asm()
		: "g" (count)
		: "%ebx", "%ecx" ); // push/pop before/after asm()
  return sid;
}
//phase 5
void MsgSnd( msg_t *msg) {
	asm("movl %0, %%ebx; int $53;" // CPU inst
		:
		:  "g" ((int)msg)
		:  "%ebx"); // push/pop before/after asm()
}

void MsgRcv(msg_t *msg) {
	asm("movl %0, %%ebx; int $54;" // CPU inst
		:
		: "g" ((int)msg)
		: "%ebx"); // push/pop before/after asm()
}

void TipIRQ3(){
	asm("int $35" );
}

void Fork(char *exe_addr)
{
	asm("movl %0, %%ebx; int $55"
	:
	:"g" ((int)exe_addr)
	:"%ebx");
}

int Wait(int *exit_num)
{
   int pid;
	asm("movl %1, %%ebx; int $56; movl %%ecx, %0;"
	:"=g" (pid)
	:"g" ((int)exit_num)
	:"%ebx","%ecx");
   return pid;
}

void Exit(int exit_num)
{
	asm("movl %0, %%ebx; int $57"
	:
	:"g" (exit_num)
	:"%ebx")

