// proc.c, 159
// processes are here

#include "spede.h"   // for IO_DELAY() needed here below
#include "extern.h"  // for current_run_pid needed here below
#include "proc.h"    // for Idle, SimpleProc, DispatchProc
#include "syscall.h"
#include <spede/machine/parallel.h>
#include "tool.h"
#include "FileMgr.h"

void Idle() {
	int i =0;
	int a;
	for(;;){
	cons_printf("%d ", i);		// print 0 on PC
	for(a=0; a<1666000; a++) IO_DELAY();   //busy-loop delay for about 1 sec
	}
} 

void UserProc() {
  int sleep;
	int p = GetPid();
	for(;;){
	 
		cons_printf("%d ", p);  // print its pid (CRP) on PC	
		sleep = 4 - (p%4);	
		Sleep(sleep);
	}
}

void PrintDriver(){
   int i, code;
   char *p;
   msg_t local_msg;
//int TIME_OUT=3*1666000;                   // time out in 3 sec
   print_semaphore = SemGet(-1);//should be -1 but depends on IRQISR

   outportb(LPT1_BASE+LPT_CONTROL, PC_SLCTIN);// printer control select interrup
   code = inportb(LPT1_BASE+LPT_STATUS);
   for(i=0; i<50; i++) IO_DELAY();
   outportb(LPT1_BASE+LPT_CONTROL, PC_INIT|PC_SLCTIN|PC_IRQEN);
   Sleep(1);

   while(1){
	cons_printf("\nmy print pid is: %d \n", GetPid() );
	MsgRcv(&local_msg);
		p = local_msg.data;
		while(*p){
			outportb(LPT1_BASE+LPT_DATA, *p);      // send char to data reg
			code = inportb(LPT1_BASE+LPT_CONTROL); // read control reg
			outportb(LPT1_BASE+LPT_CONTROL, code|PC_STROBE); // send with added strobe
			for(i=0; i<20; i++) IO_DELAY();        // delay for EPSON LP-571 printer
			outportb(LPT1_BASE+LPT_CONTROL, code); // send original control code
			SemWait(print_semaphore);
			p++;
		}//while p
	}//while1
}//print driver

void Init(){
	int pid;
	char key;
	char my_msg[] = "Hello, my Team is called TSLK \n\0";
	msg_t msg;       // local, in process space
	msg.recipient=2;
	MyStrCpy(msg.data, my_msg); //to put greeting message (to be printed) into the msg
	while(1){// infinite loop:
     		pid=GetPid();
         	cons_printf("%d ", pid);		// print 0 on PC            show my PID
		Sleep(1); 
//		for(i=0; i<1666000; i++) IO_DELAY();   //delay  1 sec               and sleep for 1 second ...
        
        if(cons_kbhit()){		//check if key hit
		key = cons_getchar();
			switch(key){
			//phase5
			case 'p':
			MsgSnd(&msg);  break;
			case 'b':breakpoint(); break;
			case 'q': exit(0);
			}
		}

	}//while
}
/*------------------------------Phase 6 -----------------------------------------------------*/
void Shell () {
   int BAUD_RATE, divisor;         // for serial port
   msg_t msg;                      // local message space
   char login[101], password[101]; // login and password strings entered
   int STDIN = 4, STDOUT = 5, FileMgr =6;      // PID's of these processes

  // initialize terminal interface data structure (below)
   MyBzero((char *) &terminal.TX_q,sizeof(q_t));
   MyBzero((char *) &terminal.RX_q,sizeof(q_t));
   MyBzero((char *) &terminal.echo_q,sizeof(q_t));//clear 3 queues: TX_q, RX_q, echo_q
   terminal.TX_sem = SemGet(Q_SIZE);   //get a semaphore to set TX_sem, count Q_SIZE (char space to terminal video)
   terminal.RX_sem = SemGet(0);   //get a semaphore to set RX_sem, count 0 (no char from terminal KB)
   terminal.echo = 1;   //set echo to 1 (default is to echo back whatever typed from terminal)
   terminal.TX_extra = 1;   //set TX_extra to 1 (an IRQ3 TXRDY event missed)

    /*
   // COM1-8_IOBASE: 0x3f8 0x2f8 0x3e8 0x2e8 0x2f0 0x3e0 0x2e0 0x260
   // transmit speed 9600 bauds, clear IER, start TXRDY and RXRDY
   // Data communication acronyms:
   //    IIR Intr Indicator Reg
   //    IER Intr Enable Reg
   //    ETXRDY Enable Xmit Ready
   //    ERXRDY Enable Recv Ready
   //    MSR Modem Status Reg
   //    MCR Modem Control Reg
   //    LSR Line Status Reg
   //    CFCR Char Format Control Reg
   //    LSR_TSRE Line Status Reg, Xmit+Shift Regs Empty
   */
   // set baud rate 9600
   BAUD_RATE = 9600;              // Mr. Baud invented this
   divisor = 115200 / BAUD_RATE;  // time period of each baud
   outportb(COM2_IOBASE+CFCR, CFCR_DLAB);          // CFCR_DLAB 0x80
   outportb(COM2_IOBASE+BAUDLO, LOBYTE(divisor));
   outportb(COM2_IOBASE+BAUDHI, HIBYTE(divisor));
   // set CFCR: 7-E-1 (7 data bits, even parity, 1 stop bit)
   outportb(COM2_IOBASE+CFCR, CFCR_PEVEN|CFCR_PENAB|CFCR_7BITS);
   outportb(COM2_IOBASE+IER, 0);
   // raise DTR, RTS of the serial port to start read/write
   outportb(COM2_IOBASE+MCR, MCR_DTR|MCR_RTS|MCR_IENABLE);
   IO_DELAY();
   outportb(COM2_IOBASE+IER, IER_ERXRDY|IER_ETXRDY); // enable TX, RX events
   IO_DELAY();	
   
   while(1){ //inifite loop: 
   
     while(1){ // loop a
     
      // prompt valid commands (send msg to STDOUT, receive reply)
       MyStrCpy(msg.data, "TSLK Shell> Commands: who, bye \n\0");
       msg.recipient = STDOUT;
       MsgSnd(&msg);
       MsgRcv(&msg);
      
       //  prompt for login (send msg to STDOUT, receive reply)
       MyStrCpy(msg.data, "TSLK Shell> login: \0");
       msg.recipient = STDOUT;
       MsgSnd(&msg);
       MsgRcv(&msg);
       
       //  get login entered (send msg to STDIN, receive reply)
	msg.recipient = STDIN;
	MsgSnd(&msg);
	MsgRcv(&msg);
	MyStrCpy(login, msg.data);
	
       //  prompt for password (same as above)
       MyStrCpy(msg.data, "TSLK Shell> password: \0");
       msg.recipient = STDOUT;
       MsgSnd(&msg);
       MsgRcv(&msg);
       
       //  get password entered (same as above)
       msg.recipient = STDIN;
       MsgSnd(&msg);
       MsgRcv(&msg);
       MyStrCpy(password, msg.data);
       
        
       if(MyStrCmp(login, password)) break; // string-compare login and password; if same, break loop A
       
       else {  //(else) prompt "Invalid login!\n\0"
       	MyStrCpy(msg.data, "TSLK Shell> Invalid login! \n\0");
       	msg.recipient = STDOUT;
       	MsgSnd(&msg);
       	MsgRcv(&msg);
       }
       
     } //end of loop a
     while(1) {//loop B:
     //prompt for entering command string
     MyStrCpy(msg.data, "TSLK Shell> enter command: \0"); 
     msg.recipient = STDOUT;
     MsgSnd(&msg);
     MsgRcv(&msg);
    
     //get command string entered
     msg.recipient = STDIN;
     MsgSnd(&msg);
     MsgRcv(&msg);
     //   if command string is empty, then continue (loop B)
      if(MyStrLen(msg.data) == 0) continue;
      else if(MyStrCmp(msg.data, "bye\0")) break; //if command string is "bye", then break (loop B)
      else if (MyStrCmp(msg.data, "who\0")) { //if command string is "whoami"
       //show login string,
       MyStrCpy(msg.data,login);
       msg.recipient = STDOUT;//not sure if this is needed here
       MsgSnd(&msg);
       MsgRcv(&msg);
       
       //and an additional "\n\0" (for aesthetics)
       msg.recipient = STDOUT;//doubly sure
       MyStrCpy(msg.data, "\n\0");
       MsgSnd(&msg);
       MsgRcv(&msg);
       continue; // continue (loop B)
      } //if command string is "whoami" (end)
      else if(MyStrCmp(msg.data, "dir\0")==1){
      	ShellDir(msg.data, STDOUT,FileMgr);
      }else if (MyStrcmpSize(msg.data,"typ",3)==1){
	ShellTyp(msg.data, STDOUT,FileMgr);
      }
      else { //else other strings are entered in command string
   	//show "Command not found!\n\0"
   	MyStrCpy(msg.data, "TSLK Shell> Command not found!\n\0");
   	msg.recipient = STDOUT;
   	MsgSnd(&msg);
   	MsgRcv(&msg);
      
     }// if command string is empty block (end)
   }//  repeat loop B
} //  repeat infinite loop
}   


void STDIN(){
char *p, ch;
msg_t msg;
	while(1){
		MsgRcv(&msg);
		p=msg.data;
		while(1){
			SemWait(terminal.RX_sem);
			ch = DeQ(&terminal.RX_q);
			if(ch=='\r') break;
			*p = ch;
			p++;
		}
		*p = '\0';
		msg.recipient = msg.sender;
		MsgSnd(&msg);
	}

}
	
void STDOUT(){
char *p;
msg_t msg;
	while(1){
		MsgRcv(&msg);
		p = msg.data;
		while(*p != '\0'){ //loop A until p points to null
			SemWait(terminal.TX_sem);
			EnQ( *p, &terminal.TX_q);
			TipIRQ3();
			if(*p=='\n'){
				SemWait(terminal.TX_sem);
				*p='\r';
				EnQ(*p, &terminal.TX_q);
			}		
		p++;
		}
	msg.recipient = msg.sender;
	MsgSnd(&msg);
	}
	
}

void ShellDirStr(attr_t *p, char *str) {
   // p points to attr_t and then obj name (p+1)
      char *obj = (char *)(p + 1);

   // make str from the attr_t that p points to
      sprintf(str, " - - - -  size =%6d     %s\n", p->size, obj);
      if ( A_ISDIR(p->mode) ) str[1] = 'd';         // mode is directory
      if ( QBIT_ON(p->mode, A_ROTH) ) str[3] = 'r'; // mode is readable
      if ( QBIT_ON(p->mode, A_WOTH) ) str[5] = 'w'; // mode is writable
      if ( QBIT_ON(p->mode, A_XOTH) ) str[7] = 'x'; // mode is executable
   }
   
void ShellDir(char *cmd, int STDOUT, int FileMgr) {
      char obj[101], str[101];
      attr_t *p;
      msg_t msg;

   // if cmd is "dir\0" (or "333\0") assume root: "dir /\0"
   // else, there should be an obj after 1st 4 letters "dir "
      if(MyStrCmp(cmd, "dir\0") == 1 || MyStrCmp(cmd, "333\0") == 1) {
         obj[0] = '/';
         obj[1] = '\0';                           // null-terminate the obj[]
      } else {
         cmd += 4;         // skip 1st 4 letters "dir " and get the rest (obj)
         MyStrCpy(obj, cmd); // make sure cmd is null-terminated from Shell()
      }

   //*************************************************************************
   // write code:
   // apply standard "check object" protocol
   
   //    prep msg: put correct code and obj into msg
   	MyStrCpy(msg.data, obj);
   	msg.code = CHK_OBJ;
   //    send msg to FileMgr, receive reply, chk result code
   	msg.recipient= FileMgr;
	MsgSnd(&msg);
	MsgRcv(&msg);
   // if code is not GOOD
   if(msg.code != GOOD){
   //    prompt error msg via STDOUT
   MyStrCpy(msg.data, "There was an error with the msg.code. \n\0");
   //    receive reply
   msg.recipient = STDOUT;
   MsgSnd(&msg);
   MsgRcv(&msg);
   //    return;        // cannot continue
   return;
   }
   //*************************************************************************
	
	
	
	
   //*************************************************************************
   // otherwise, code is good, returned msg has an "attr_t" type,
   // check if user directed us to a file, then "dir" for that file;
   // write code:
   		p = (attr_t *) msg.data;
   		if( ! A_ISDIR(p->mode) ) {
   			ShellDirStr(p, str);        // str will be built and returned
   //    prep msg and send to STDOUT
   			MyStrCpy(msg.data, str);
   			msg.recipient = STDOUT;
   			MsgSnd(&msg);
   //    receive reply
   			MsgRcv(&msg);
   			
   //    return;
   			return;
   		}
   
   //*************************************************************************

   //*************************************************************************
   // if continued here, it's a directory
   // request to open it, then issue read in loop
   // write code:
   // apply standard "open object" protocol
   MyStrCpy(msg.data, obj);
   msg.code=OPEN_OBJ;
   // prep msg: put code and obj in msg
     // send msg to FileMgr, receive msg back (should be OK)
   msg.recipient = FileMgr;
   MsgSnd(&msg);
   MsgRcv(&msg);
   //
   // loop
   
   //    apply standard "read object" protocol
   	while(1){
		MyStrCpy(msg.data, obj);//(attr_t *)msg.data->data
   		msg.code=READ_OBJ;
   		msg.recipient= FileMgr;
   //    prep msg: put code in msg and send to FileMgr
   //    receive reply
   	
   		MsgSnd(&msg);
   		MsgRcv(&msg);
   		
   		if(msg.code !=GOOD)  break;  //    if code came back is not GOOD, break loop

    		//p = (attr_t *)msg.data;
   		ShellDirStr(p, str);
		MyStrCpy(msg.data, str);
   		msg.recipient= STDOUT;
   		MsgSnd(&msg);
   		MsgRcv(&msg);

   	};
   
   //    (if continued, code was good)
   //    do the same thing with ShellDirStr() like above
   //    then show str via STDOUT
   // }
   MyStrCpy(msg.data, obj);
   msg.code=CLOSE_OBJ;
   msg.recipient = FileMgr;
   MsgSnd(&msg);
   MsgRcv(&msg);
   


   if(msg.code !=GOOD){
   	MyStrCpy(msg.data, "The request was not valid.\n\0");
   	msg.recipient=STDOUT;
   	MsgSnd(&msg);
   	MsgRcv(&msg);
   	//promptuser
   }
   //*************************************************************************
   //*************************************************************************
   // write code:
   // (abbreviated below since same as what done above)
   // apply standard "close object" protocol with FileMgr
   // if response is not GOOD, display error msg via STDOUT...
   //*************************************************************************

      }

  void ShellTyp(char *cmd, int STDOUT, int FileMgr) {
      // char obj[101], str[101]; // get away without obj
      attr_t *p;
      msg_t msg;
      
   // write code:
    cmd += 4;         // skip 1st 4 characters in cmd ("typ ") to get rest
    MyStrCpy(msg.data, cmd);  // copy rest to msg.data
   
   msg.recipient=FileMgr; // ask FileMgr to check object on this
   msg.code = CHK_OBJ;
   
   MsgSnd(&msg);
   MsgRcv(&msg);
   
   p = (attr_t *)msg.data;
   if(msg.code != GOOD ||  A_ISDIR(p->mode)){// if result not GOOD, or p->mode is a directory
		MyStrCpy(msg.data, "TSLK Shell> Usage: [path]<filename>\n\0"); //    display "Usage: typ [path]<filename>\n\0"
       	msg.recipient = STDOUT;
       	MsgSnd(&msg);
       	MsgRcv(&msg);

        return;
   }

   MyStrCpy(msg.data, cmd);  // copy rest to msg.data
   msg.recipient=FileMgr; // ask FileMgr to check object on this
   msg.code = OPEN_OBJ;
   
   MsgSnd(&msg);
   MsgRcv(&msg);
   
   // (below much similar as what done above:)
   // otherwise, a file, then show its content: request open
	while(1) { // loop to read
		 msg.recipient=FileMgr;
		 msg.code = READ_OBJ;
		 MsgSnd(&msg);
		 MsgRcv(&msg);
		 
		 if(msg.code != GOOD ) break; //    break if can read not good

		msg.recipient = STDOUT; //    display what's read via STDOUT
		MsgSnd(&msg);
		MsgRcv(&msg);
	}
   
	// request to close FD
	msg.recipient=FileMgr;
	msg.code = CLOSE_OBJ;
	MsgSnd(&msg);
	MsgRcv(&msg);

	if( msg.code != GOOD )
	{
		MyStrCpy(msg.data, "TSLK Shell> Error: Cannot Close fd object\n\0"); //    display "Error: Cannot Close fd object\n\0"
		msg.recipient = STDOUT;
		MsgSnd(&msg);
		MsgRcv(&msg);
	}
  }
