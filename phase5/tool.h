// tool.h, 159

#ifndef _TOOL_H
#define _TOOL_H

#include "type.h" // q_t needs be defined in code below
int EmptyQ(q_t *);
int FullQ(q_t *);
void MyBzero(char *, int);
void MyBZero(void *, int);
int DeQ(q_t *);
void EnQ(int, q_t *);
void initq(q_t *);


#endif

