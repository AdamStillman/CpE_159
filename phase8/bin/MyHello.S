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
   pushl %esp               #(copy the stack pointer:)
   #push the stack pointer
   
   #pop into register ebx
   #subtract 4096 from it (this is the base, real addr of the page)

   #copy $msg to register ecx
   #subtract 2G from it, get x (offset)

   #add  x (offset) to ebx (base of page) -- where msg really is
   #save a copy (push it to stack)
   #save another copy (push it again)

   #call interrupt number 53  # MsgSnd(&msg)

   #pop to ebx (get a copy of real msg addr)
   #call interrupt number 54  # MsgRcv(&msg)

   #pop to ecx (get a copy, real msg addr)
   #copy time stamp (base ecx + offset of time stamp) to ebx
   #call interrupt number 57  # Exit(time stamp)

.data                       # data segment follows code segment in RAM
msg:                        # my msg
                            # msg.sender
                            # msg.recipient
                            # msg.time_stamp
                            # msg.data
                            # etc.
          
