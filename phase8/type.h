// type.h, 159

#ifndef _TYPE_H_
#define _TYPE_H_

#include "Tf.h"

#define TIME_LIMIT 300        // max timer count to run
#define MAX_PROC 20          // max number of processes
#define Q_SIZE 20            // queuing capacity
#define STACK_SIZE 4096      // process stack in bytes

// this is the same as constants defined: KMODE=0, UMODE=1
typedef enum {KMODE, UMODE} mode_t;
typedef enum {NONE, RUNNING, RUN, SLEEP, WAIT, ZOMBIE} state_t;

typedef struct {             // PCB describes proc image
   mode_t mode;              // process privilege mode
   state_t state;            // state of process
   int runtime;              // run time since dispatched
   int total_runtime;        // total run time since created
   int wake_time;
   TF_t *TF_ptr;
} pcb_t;

typedef struct {             // proc queue type
   int head, tail, size;     // where head and tail are, and current size
   int q[Q_SIZE];            // indices into q[] array to place or get element
} q_t;


typedef void (* func_ptr_t)(); // void-returning function pointer type

typedef struct{ //Used for semaphores
  int count;
  q_t wait_q;
} semaphore_t;

typedef struct {
  int sender,
  recipient,
  time_stamp,
  code,
  number[3];
  char data[101];
} msg_t;

typedef struct{
  msg_t msg[Q_SIZE];
  int head, tail, size;
} msg_q_t;

typedef struct{
  msg_q_t msg_q;
  q_t wait_q;
} mbox_t;

 typedef struct {             // Phase 6
      q_t TX_q,     // to transmit to terminal
          RX_q,     // to receive from terminal
          echo_q;   // to echo back to terminal
      int TX_sem,   // transmit space available count
          RX_sem,   // receive data (arrived) count
          echo,     // to echo back to terminal (the typing) or not
          TX_extra; // if 1, TXRDY event occurred but echo_q and TX_q were empty
   } terminal_t;


#endif _TYPE_H_
