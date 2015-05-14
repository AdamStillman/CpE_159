// extern.h, 159

#ifndef _EXTERN_H_
#define _EXTERN_H_

#include "type.h"            // q_t, pcb_t, MAX_PROC, STACK_SIZE

extern int CRP, sys_time;              // PID of currently-running process, -1 means none
extern q_t run_q, none_q, sleep_q;    // ready to run, not used proc IDs
extern pcb_t pcb[MAX_PROC];  // process table
extern char stack[MAX_PROC][STACK_SIZE]; // proc run-time stacks

extern int print_semaphore;
extern int print_it, semaphoreID;
extern semaphore_t semaphore[MAX_PROC];
extern q_t semaphore_q;
extern mbox_t mbox[MAX_PROC];
//phase6
extern terminal_t terminal;
extern page_t page[MAX_PROC];

#endif
