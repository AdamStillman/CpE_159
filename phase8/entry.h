// entry.h, 159

#ifndef _ENTRY_H_
#define _ENTRY_H_

#include <spede/machine/pic.h>

#define TIMER_INTR 32
#define GETPID_INTR 48
#define SLEEP_INTR 49
#define SEMWAIT_INTR 50
#define SEMPOST_INTR 51
#define SEMGET_INTR 52
#define IRQ7_INTR 39
#define MSGSND_INTR 53
#define MSGRCV_INTR 54
#define IRQ3_INTR 35
#define FORK_INTR 55
#define WAIT_INTR 56
#define EXIT_INTR 57

#define KCODE 0x08    // kernel's code segment
#define KDATA 0x10    // kernel's data segment
#define KSTACK_SIZE 8192  // kernel's stack size, in chars

// ISR Entries
#ifndef ASSEMBLER

__BEGIN_DECLS

#include "type.h" // for trapframe_t below

extern void TimerEntry();     // code defined in entry.S
extern void Dispatch(TF_t *); // code defined in entry.S
extern void GetPidEntry();
extern void SleepEntry();
extern void SemWaitEntry();
extern void SemPostEntry();
extern void SemGetEntry();
extern void IRQ7Entry();
extern void MsgSndEntry();
extern void MsgRcvEntry();
extern void IRQ3Entry();
__END_DECLS

#endif

#endif
