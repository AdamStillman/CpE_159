// proc.h, 159

#ifndef _PROC_H_
#define _PROC_H_


void Idle();
void UserProc();
void PrintDriver();
void Init();
void Shell();
void STDIN();
void STDOUT();
//void ShellDirStr(attr_t , char *);
void ShellDir(char *, int, int);
void ShellTyp(char *, int, int); 

#endif
