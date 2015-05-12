// entry.S, 159
// entry points of interrupts

#include <spede/machine/asmacros.h>
#include "entry.h"

// set DS and ES to Kernel's data segment
#define SET_KSEGS movw $(KDATA), %ax; mov %ax, %ds; mov %ax, %es

.comm kstack, KSTACK_SIZE, 1 // define kernel stack space
.text

// Dispatch() loads registers from trapframe to run
ENTRY(Dispatch)
   movl 4(%esp), %eax   // load stack pointer from eax
   movl %eax, %esp
   popl %gs
   popl %fs
   popl %es
   popl %ds
   popa                 // restore general registers
   add $4, %esp         // skip 4 bytes that stored intr_num
   iret

ENTRY(SleepEntry)
   pushl $SLEEP_INTR
   jmp IntoKernel

ENTRY(GetPidEntry)
   pushl $GETPID_INTR
   jmp IntoKernel

// push intr type number then jump to common handler
ENTRY(TimerEntry)
   pushl $TIMER_INTR
   jmp IntoKernel

ENTRY(SemWaitEntry)
   pushl $SEMWAIT_INTR
   jmp IntoKernel

ENTRY(SemPostEntry)
   pushl $SEMPOST_INTR
   jmp IntoKernel

ENTRY(SemGetEntry)
   pushl $SEMGET_INTR
   jmp IntoKernel

ENTRY(IRQ7Entry)
   pushl $IRQ7_INTR
   jmp IntoKernel

ENTRY(MsgSndEntry)
   pushl $MSGSND_INTR
   jmp IntoKernel
   
ENTRY(MsgRcvEntry)
   pushl $MSGRCV_INTR
   jmp IntoKernel
   
ENTRY(IRQ3Entry)
   pushl $IRQ3_INTR
   jmp IntoKernel
   
ENTRY(ForkEntry)
   pushl $FORK_INTR
   jmp IntoKernel

ENTRY(WaitEntry)
   pushl $WAIT_INTR
   jmp IntoKernel

ENTRY(ExitEntry)
   pushl $EXIT_INTR
   jmp IntoKernel
   
   
// Common IRQ entry, save context and call Kmain()
IntoKernel:
   pusha
   pushl %ds
   pushl %es
   pushl %fs
   pushl %gs
   movl %esp, %edx
   cld
   SET_KSEGS      // set DS and ES to kernel's data segments
   leal kstack + KSTACK_SIZE, %esp
   pushl %edx
   call CNAME(Kernel)
