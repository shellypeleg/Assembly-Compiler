.entry LOOP
MAIN: inc K
	mov S1.2, W
.extern W
	add r2,STR
LOOP: jmp W
	prn #-5
STR: .string "ab"
	sub r1,r4
	stop
.entry STR
K: .data 2,7
S1: .struct 7, "hello"	
