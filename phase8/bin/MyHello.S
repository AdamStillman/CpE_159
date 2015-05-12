# MyHello.s
#
# in order to perform syscall MsgSnd(&msg), MsgRcv(&msg), need to
# first calculate where the real RAM addr of my msg is:
# use ESP - 4096 to get the base of the 4KB RAM page this program resides.
# Since $msg is virtual, at 2G+x (0x80000000+x), and the page is virtual
# at 2G (0x80000000); subtract 2G from $msg gets x. Add x to the base is
# the real RAM addr of the msg.

.text                       # code segment
.global _start              # _start is main()

_start:                     # instructions begin
                            # 1st, get real addr of local msg
   #(copy the stack pointer:)
   pushl %esp               #push the stack pointer
   pop %ebx                 #pop into register ebx
   sub $0x1000, %ebx        #subtract 4096 from it (this is the base, real addr of the page)

   movl $msg, %ecx          #copy $msg to register ecx
   sub 0x80000001, %ecx     #subtract 2G from it, get x (offset)

   add %ecx, %ebx           #add  x (offset) to ebx (base of page) -- where msg really is
   pushl %ebx               #save a copy (push it to stack)
   pushl %ebx               #save another copy (push it again)
   
   int $53                    # MsgSnd(&msg)

   popl %ebx                #pop to ebx (get a copy of real msg addr)
   int $54                  #call interrupt number 54  # MsgRcv(&msg)

   popl %ecx                #pop to ecx (get a copy, real msg addr)
   mov 8(%ecx), %ebx        #copy time stamp (base ecx + offset of time stamp) to ebx
   
   int $57                  #call interrupt number 57  # Exit(time stamp)

.data                       # data segment follows code segment in RAM
msg:                        # my msg
    .long 0                 # msg.sender
    .long 5                 # msg.recipient
    .rept 5                 # msg.time_stamp
    .long 0                 # msg.data
    .endr                   # etc.
          
    .ascii "Hello from TSLK\n" #msg.data (16 char)
    .rept 85                #101-16 = 85
    .ascii "\0"             #null chars
    .endr                   #end repeat
    
