# MySleep.s
# here it Sleep for 3 seconds as a demo, change it to the same
# number as its process ID (e.g., if PID is 7, Sleep(7) will occur)
#
# To compile and extract an executable are shell commands below.
# Run shell script "doit MySleep" instead. (If commands mistyped,
# erasure of source code may occur!)
#    as --32 this_file_name.s -o this_file_name.o 
#    link386 -nostartfiles -userapp this_file_name.o -o this_file_name
#    extract.pl this_file_name (finally get this_file_name.hex)

# If curious about identifier table in code: as -a this_file_name.s

.text               # code segment 
.global _start      # _start is gonna be main()

_start:             # instructions starts
  # movl $3, %ebx    # seconds to sleep
   int  $48         # call get pid
   int  $49         # call sleep service

   movl $99, %ebx   # exit number
   int  $57         # call exit service

.data               # need no data, but some examples below
x:
   .long 1          # an integer, 100 initially
str1:
   .ascii "ABCDEFGH!\n\0" # a string
str2:                     # another string, 101 chars total
   .ascii "Greetings from TSLK!\n"    # 21 chars
   .rept 73               # repeat 69 times
      .ascii "\0"         # null characters
   .endr                  # end repeat





