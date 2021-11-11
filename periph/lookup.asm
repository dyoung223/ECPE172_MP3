;; Boilerplate code for lookup procedure

        .global  lookup

        .thumb                          ; generate Thumb-2 code
        .text                           ; switch to code (ROM) section

;; Place declarations and references for variables here
        .align  4                       ; force word alignment

        .global lookup

;; Place code for lookup here
        .align  4                       ; force word alignment
lookup:  .asmfunc
        push    {LR, R4-R12}                    ; save return address

        mov 	R4, R0					; R0 is row
        mov		R5, R1					; R1 is column
        mov		R6, R2					; R2 is size
        mov 	R7, R3					; R3 is table

        cmp		R4, R6
        bhs		outofrange
        cmp		R5, R6
        bhs		outofrange

        mul		R4, R6
        add		R4, R5

        ldr		R8, [R7, R4]
        mov 	R0, R8
		b 		end

outofrange:

        mov 	R0, #0xff

end:
        pop     {PC, R4-R12}                    ; return to caller

        .endasmfunc
        .end

