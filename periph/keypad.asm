;;
;; keypad.asm: Boilerplate code for keypad procedures
;;
;;  Created on:  July 21, 2019
;;      Author: khughes
;;

        .thumb                          ; generate Thumb-2 code

		.global	initRCGCGPIO
		;.global setrow
		.global cport, rport, lastrow
        .text                           ; switch to code (ROM) section

;; Place declarations and references for variables here.

		;.global	cport, rport, lastrow

;;		since the rcgcgpio register is 32 bits, declare a uint32_t
;;      (4 byte) variable with word alighment (4)



        .align  4                       ; force word alignment
;; Place definitions for peripheral registers here (base
;; addresses, register offsets, and field constants).  Follow the
;; example for the SYSCTL RCGCCPIO register from Lab 3.
addr_rport: 			.word 	rport
addr_cport:				.word 	cport
addr_lastrow: 			.word	lastrow
SYSCTL:                 .word   0x400fe000
SYSCTL_RCGCGPIO         .equ    0x608
SYSCTL_RCGCGPIO_PORTL   .equ    (1<<10)
GPIO_PORTL:             .word   0x40062000
GPIO_DIR				.equ	0x400
GPIO_PUR				.equ	0x510
GPIO_DEN				.equ	0x51C
GPIO_ODR				.equ	0x50C
GPIO_AFSEL				.equ 	0x420
GPIO_PIN_0              .equ    (1 << 0)
GPIO_PIN_1              .equ    (1 << 1)
GPIO_PIN_2				.equ 	(1 << 2)
GPIO_PIN_3				.equ	(1 << 3)

;; void initKeypad( const struct portinfo *col, const struct portinfo *row );
		.global initKeypad
        .align  4                       ; force word alignment
initKeypad:   .asmfunc
	push    {LR,R5-R12}             ; save return address and other registers

	;;// temporary varaiable representing row or column port pins
	;;  uint8_t pins;

	;;  // Save base addresses for row and column ports for getKey()
	;;  rport = r;
	;;  cport = c;
	ldr			R5, addr_cport		; load address of cport
	ldr			R6, addr_rport		; load address of rport
	str			R0, [R5]		; move the input parameters into variables, c ; R5
	str			R1, [R6]		; move the input parameters into variables, r ; R6


	;;  // enable port clocks
	;;  initRCGCGPIO( cport->base );
	;;  initRCGCGPIO( rport->base );
	ldr		R7, [R5]		;R7 is cport
	ldr		R0, [R7, #4] 	;cport four addresses or 4 byte aligns to get to the base address
	bl 		initRCGCGPIO

	ldr 	R8, [R6]		;R8 is rport
	ldr		R0, [R8, #4] 	;rport four addresses or 4 byte aligns to get to the base address
	bl 		initRCGCGPIO


	;;  // enable row pins as outputs
	;;  pins = rport->pins[0] | rport->pins[1] | rport->pins[2] | rport->pins[3];

	ldrb	R10, [R8, #0]				;pull out pins[0:3] from rport
	orr 	R9, R10, R10					;sets R9 to pin[0]
	ldrb 	R10, [R8, #1]
	orr		R9, R9, R10						;sets R9 to pin[0]|pin[1]
	ldrb	R10, [R8, #2]
	orr		R9, R10, R9						;sets R9 to pin[0]|pin[1]|pin[2]
	ldrb	R10, [R8, #3]
	orr 	R9, R10, R9						;sets R9 to pin[0]|pin[1]|pin[2]|pin[3]


	;;  // set direction as output
	ldr		R11, [R8, #4]
	;;  rport->base[GPIO_DIR] |= pins;
	ldr 	R10, [R11, #GPIO_DIR]			;load the current GPIO_DIR data
	orr 	R10, R9							;set pins
	str		R10, [R11, #GPIO_DIR]			;return data back into the register

	;;  // turn off pull-ups
	;;  rport->base[GPIO_PUR] &= ~pins;
	ldr 	R10, [R11, #GPIO_PUR]			;load current GPIO_PUR data
	bic		R10, R9
	str		R10, [R11, #GPIO_PUR]


	;;  // ensure open-drain
	;;  rport->base[GPIO_ODR] |= pins;
	ldr 	R10, [R11, #GPIO_ODR]			;load the current GPIO_ODR data
	orr 	R10, R9							;set pins
	str		R10, [R11, #GPIO_ODR]			;return data back into the register

	;;  // turn off alternate functions
	;;  rport->base[GPIO_AFSEL] &= ~pins;
	ldr 	R10, [R11, #GPIO_PUR]			;load curretn GPIO_AFSEL data
	bic		R10, R9
	str		R10, [R11, #GPIO_PUR]


	;;  // enable
	;;  rport->base[GPIO_DEN] |= pins;
	ldr 	R10, [R11, #GPIO_DEN]			;load the current GPIO_DEN data
	orr 	R10, R9							;set pins
	str		R10, [R11, #GPIO_DEN]			;return data back into the register


	;;  // enable col pins as inputs
	;;  pins = cport->pins[0] | cport->pins[1] | cport->pins[2] | cport->pins[3];
	ldrb	R10, [R7, #0]					;pull out pins[0:3] from rport
	orr 	R9, R10, R10					;sets R9 to pin[0], should clear previous, if not use bfc (bit field clear)
	ldrb 	R10, [R7, #1]
	orr		R9, R9, R10						;sets R9 to pin[0]|pin[1]
	ldrb	R10, [R7, #2]
	orr		R9, R10, R9						;sets R9 to pin[0]|pin[1]|pin[2]
	ldrb	R10, [R7, #3]
	orr 	R9, R10, R9						;sets R9 to pin[0]|pin[1]|pin[2]|pin[3]

	;;  // set direction as input
	ldr		R11, [R7, #4] 					;store cport->base
	;;  cport->base[GPIO_DIR] &= ~pins;
	ldr		R10, [R11, #GPIO_DIR]
	bic		R10, R9
	str		R10, [R11, #GPIO_DIR]

	;;  // enable pull-ups
	;;  cport->base[GPIO_PUR] |= pins;
	ldr		R10, [R11, #GPIO_PUR]
	orr		R10, R9
	str		R10, [R11, #GPIO_PUR]

	;;  // turn off open-drain
	;;  cport->base[GPIO_ODR] &= ~pins;
	ldr		R10, [R11, #GPIO_ODR]
	bic		R10, R9
	str		R10, [R11, #GPIO_ODR]

	;;  // turn off alternate functions
	;;  cport->base[GPIO_AFSEL] &= ~pins;
	ldr		R10, [R11, #GPIO_AFSEL]
	bic		R10, R9
	str		R10, [R11, #GPIO_AFSEL]

	;;  // enable
	;;  cport->base[GPIO_DEN] |= pins;
	ldr		R10, [R11, #GPIO_DEN]
	orr		R10, R9
	str		R10, [R11, #GPIO_DEN]

	;;  // set the initial row to scan
	mov 	R0, #0
	;;  setrow( 0 );
	bl		setrow

	pop     {PC,R5-R12}             ; restore all register and return
        .endasmfunc



;; bool getKey( uint8_t *col, uint8_t *row );
		.global getKey
        .align  4                       ; force word alignment


       ;;bool getKey( uint8_t *col, uint8_t *row )
getKey: .asmfunc
	push    {LR,R5-R12}             ; save return address and other registers
	;;	// Get pin numbers for columns

	ldr		R5, addr_cport		; load address of cport

	ldr		R7, [R5]		; cport

	;;  uint8_t pins = cport->pins[0] | cport->pins[1] | cport->pins[2] | cport->pins[3];
	ldrb	R10, [R7, #0]				;pull out pins[0:3] from rport
	orr 	R9, R10, R10				;sets R9 to pin[0], should clear previous, if not use bfc (bit field clear)
	ldrb 	R10, [R7, #1]
	orr		R9, R9, R10					;sets R9 to pin[0]|pin[1]
	ldrb	R10, [R7, #2]
	orr		R9, R10, R9					;sets R9 to pin[0]|pin[1]|pin[2]
	ldrb	R10, [R7, #3]
	orr 	R9, R10, R9					;sets R9 to pin[0]|pin[1]|pin[2]|pin[3]

	;;  // If all pins are high, change the row for the next time.
	ldr		R11, [R7, #4] 	;cport->base  four addresses or 4 byte aligns to get to the base address
	lsl		R6, R9, #2
	ldr		R10, [R11, R6]		;cport->base[pins]
	;;  if( cport->base[pins] == pins ) {
	cmp 	R10, R9				;compare cport->base[pins] with pins
	beq 	cportEqPins			;if equal
	;;    setrow( lastrow + 1 );
	;;    return false;
	;;  }

	;;  // Otherwise one column is low....

	;;  // Return row number
	;;  *row = lastrow;
	ldr 	R11, addr_lastrow		;value of lastrow
	ldrb	R11, [R11]				;value at last row
	strb	R11, [R1]				;store data at lastrow into the memory location of row

	mov		R8, #0 	;i = 0
	;;  // Now figure out which column it is
	;;  // Only check three columns; if none of them are low, it has to be the
	;;  // fourth.
	;;  for( uint8_t i = 0; i < 3; ++i ) {
	b 		forloop
forloop:
	ldr		R12, [R7, #4] 		;cport->base  four addresses or 4 byte aligns to get to the base address
	ldrb 	R6, [R7, R8]		;cport->pins[i]
	lsl		R6, R6, #2
	ldr		R10, [R12, R6]		;cport->base[cport->[pins[i]]]
	cmp		R10, #0				;if cport->base[cport->[pins[i]] == 0
	;;    if( cport->base[cport->pins[i]] == 0 ) {
	beq 	returnCol
	;;      *col = i;
	;;      return true;
	;;    }
	;;  }
	add		R8, #1				;i++
	cmp 	R8, #3 				;if i >= 3 exit loop
	bhs 	EndFor				;go to end of for loop else return back to top of for loop
	b 		forloop
	;;	if i >= 3
	;;  *col = 3;
	;;  return true;
returnCol:
	;;      *col = i;
	strb 	R8, [R0]			;store i in col
	mov 	R0, #1				;return true

	b 		EndCond

EndFor:
	;;  *col = 3;
	;;  return true;
	mov 	R4, #3
	strb	R4, [R0]
	mov		R0, #1

	b 		EndCond


cportEqPins:
	;;setrow( lastrow + 1 );
	ldr 	R11, addr_lastrow
	ldrb	R6, [R11]
	add 	R6, R6, #1
	mov 	R0, R6
	bl 		setrow
	;;    return false;
	mov		R0, #0
	b 		EndCond
EndCond:

	pop     {PC,R5-R12}             ; restore all register and return
    .endasmfunc



		.global setrow
        .align  4                       ; force word alignment
setrow: .asmfunc
	push    {LR,R5-R12}             ; save return address and other registers
	ldr		R6, addr_rport			; load address of rport
	ldr 	R8, [R6]				;R8 is rport
	ldrb	R10, [R8, #0]				;pull out pins[0:3] from rport
	orr 	R9, R10, R10					;sets R9 to pin[0]
	ldrb 	R10, [R8, #1]
	orr		R9, R9, R10						;sets R9 to pin[0]|pin[1]
	ldrb	R10, [R8, #2]
	orr		R9, R10, R9						;sets R9 to pin[0]|pin[1]|pin[2]
	ldrb	R10, [R8, #3]
	orr 	R9, R10, R9						;sets R9 to pin[0]|pin[1]|pin[2]|pin[3]

	and 	R7, R0, #3
	ldr		R11, addr_lastrow
	str		R7, [R11]

	ldr		R11, [R8, #4]			;rport->base
	ldr		R10, [R8, R7]			;rport->pins[lastrow]
	eor		R7, R9, R10				;pins xor rport->pins[lastrow]
	str		R7, [R11, R9, lsl #2]


	pop     {PC,R5-R12}             ; restore all register and return
    .endasmfunc


        .data                           ; switch to data (RAM) section
        .align  4                       ; force word alignment
;cport:					.word	0x00000000
;rport:					.word 	0x00000000
;lastrow:				.byte	0x00

		.align 4

;; Place any variables stored in RAM here

        .end
s
