// syscall.h

#ifndef _SYSCALL_H_
#define _SYSCALL_H_
#include "type.h"

int GetPid();      // no input, 1 return
void Sleep(int);   // 1 input, no return
void SemWait(int);
void SemPost(int);
int SemGet(int);
void MsgSnd(msg_t *);
void MsgRcv(msg_t *);
void TipIRQ3();
//phase 8
void Fork(char *);
int Wait(int *);
void Exit(int);

#endif
