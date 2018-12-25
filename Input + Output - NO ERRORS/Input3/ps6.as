


.entry HI
.extern BYE

;comment

WhAT: 		cmp r4,r1
	bne BYE
		mov S4.2, BYE
;another comment
HI: prn #63

S4: .struct 64, "example"
