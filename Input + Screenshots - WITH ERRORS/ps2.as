;file ps2.as




.entry LOOP
.entry LENGTH
.extern L3
.extern W

;another comment
MAIN:		mov S1.1,#-5
		add r2,STR
LOOP:		jmp W
		prn #-58888
		sub r1,r4
		inc K

		mov S1.3,r3
		bne L3
END:		spot
STR:		.string "abcdef
LENGTH: 	.data 6,-9,15
K:		.data blabla
S1:		.struct 8,"ab"
