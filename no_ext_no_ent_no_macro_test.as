MAIN: mov S1.1, r0
add r2,STR
LOOP: jmp r7
prn #-5
sub r1, r4
inc K


mov S1.2 ,r3
bne r4
END: hlt
STR: .string "abcdef"
LENGTH: .data 6,-9,15
K: .data 22
S1: .struct 8, "ab"
