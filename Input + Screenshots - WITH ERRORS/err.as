; err.as
abcdef1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678
STR :		.string 	"abcdef"
STR2:.string 	"abcdef"
str:			.string 	"abcdef1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678"

STR3:		. string 	"abcdef
			.string 	asdfghj
LENGTH:		.data		6,-9 , 15 , 1230
LENGTH1:	.data		6,-9 , 15 23
LENGTH2:	.data		
LENGTH:		.data		6,-9 , 15  
K:			.data		22, a
S1:			.struct		8 "ab"
S2:			.struct		5, "test" , "sd"
			.struct		1238 ,"ab"
			.struct		a", "ab"
La:			.entry		LOOP
			.extern		L3, W
			.extern		W
			.DATA		87
LAB:
MAIN:		mov			S1,1,W
			mov 		S1.3 ,r3
			mov 		r3
			mov 		r3, #5
			cmp			#56, W
			add			#23, r4
			add			r2,STR
MAIN:		sub			r2, S1.1
			sub			r1, r4 , r3
			not			#5
			not			
			Clr			r6
			lea			LENGTH, L3
			lea			S1.1, L3
			inc			S2.2
			dec			K
			jmp			end
			bne			LENGTH
			red			r1
			prn			#1234
			jsr			K
			rts			
			add			r2,STR
LOOP:		jmp			W
			prn 		#-5
			inc 		K

			bne 		L3
END:		stop		W

