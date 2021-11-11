;;
;; keypad.asm: Boilerplate code for keypad procedures
;;
;;  Created on:  July 21, 2019
;;      Author: khughes
;;

        .thumb                          ; generate Thumb-2 code

		.global initRCGCGPIO
        .global initKeypad
        .global getKey
        .global setrow
        .global cport, rport, lastrow

        .text                           ; switch to code (ROM) section

;; Place declarations and references for variables here.

        .align  4                       ; force word alignment
;; Place definitions for peripheral registers here (base
;; addresses, register offsets, and field constants).  Follow the
;; example for the SYSCTL RCGCGPIO register from Lab 3.

SYSCTL:                 .word   0x400fe000
SYSCTL_RCGCGPIO         .equ    0x608
SYSCTL_RCGCGPIO_PORTA   .equ    (1<<0)
SYSCTL_RCGCGPIO_PORTH   .equ    (1<<7)
SYSCTL_RCGCGPIO_PORTC 	.equ 	(1<<2)
GPIO_PORTA: 			.word 	0x40058000
GPIO_PORTH:				.word	0x4005f000
GPIO_PORTC:				.word	0x4005a000
GPIO_DIR				.equ	(0x400)
GPIO_PUR				.equ	(0x510)
GPIO_DEN				.equ	(0x51c)
GPIO_LOCK				.equ	(0x520)
GPIO_CR					.equ	(0x524)
GPIO_ODR				.equ	(0x50c)
GPIO_AFSEL				.equ	(0x420)
GPIO_PIN_0				.equ	(1<<0)
GPIO_PIN_1				.equ	(1<<1)
GPIO_PIN_2				.equ	(1<<2)
GPIO_PIN_3				.equ	(1<<3)
GPIO_PIN_4				.equ	(1<<4)
GPIO_PIN_5				.equ	(1<<5)
GPIO_PIN_6				.equ	(1<<6)
GPIO_PIN_7				.equ	(1<<7)
PIN0					.equ	0x0
PIN1					.equ 	0x1
PIN2					.equ	0x2
PIN3					.equ	0x3
BASE					.equ	0x4
RPORT_ADDR:				.word 	rport
CPORT_ADDR:				.word	cport
LASTROW_ADDR:			.word	lastrow

;; static void initRCGCGPIO( volatile uint32_t * base);

		.align 4

initRCGCGPIO:
		.asmfunc							; initializes the RCGCGPIO for the port passed into the function

		push 	{LR, R4-R12}

		mov 	R4, R0						; move base addr to R4
		ldr 	R0, SYSCTL					; load base address of sysctl into R0
		ldr		R1, [R0, #SYSCTL_RCGCGPIO]	; load all sysctl stuff
		ldr 	R2, GPIO_PORTA				; load port A offset into R2
		sub		R4, R2						; subtract PortA base addr from passed base addr, store in R4
		lsr		R4, #12						; logical shift right 12 bits
		mov 	R5, #1						; load 1 into R5
		lsl		R5, R4						; logical shift left by the subtracted value in R4
		orr		R1, R5						; bitwise or RCGCGPIO with the final value to enable port clock
		str		R1, [R0, #SYSCTL_RCGCGPIO]	; store back into sysctl

		pop		{PC, R4-R12}

		.endasmfunc


		.align 	4

;;setrow:
;;		.asmfunc

;;		push 	{LR, R4-R12}



;;		pop		{PC, R4-R12}

;;		.endasmfunc


;; void initKeypad( const struct portinfo *col, const struct portinfo *row );

        .align  4                       ; force word alignment

initKeypad:
		.asmfunc
		push	{LR, R4-R12}

		mov 	R4, R0						; moving arguments *col R4
		mov		R5, R1						; *row to R5

		ldr		R6, CPORT_ADDR				; move address of cport into R6 register
		ldr		R7, RPORT_ADDR				; move address of rport into R7 register

		str 	R0, [R6]					; put contents of *col into CPORT at the address
		str		R1, [R7]					; put contents of *row into RPORT at the address

		ldr 	R0, [R4, #BASE]				; loading address of base of *col
		bl 		initRCGCGPIO				; turns on RCGCGPIO for PORTH

		ldr		R0, [R5, #BASE]				; loading address of base of *row
		bl		initRCGCGPIO				; turns on RCGCGPIO for PORTC

;; for row

		ldrb	R0, [R5, #PIN0]				; load each pin into R0-R3 to be or'ed
		ldrb	R1, [R5, #PIN1]
		ldrb	R2, [R5, #PIN2]
		ldrb	R3,	[R5, #PIN3]

		orr		R0, R1						; orr each of the pins to create the bitmask
		orr 	R0, R2
		orr		R0, R3

		ldr		R1, [R5, #BASE]				; reload base address of col back into R1
		ldr		R2, [R1, #GPIO_DIR]			; get value stored at GPIO_DIR and load into R2
		orr		R3, R0, R2					; orr value in GPIO_DIR and bitmask (R0) and store in R3
		str 	R3, [R1, #GPIO_DIR]			; store back out to GPIO_DIR

		ldr		R2, [R1, #GPIO_PUR]			; do same process as above with PUR register
		bic		R3, R0, R2
		str 	R3, [R1, #GPIO_PUR]

		ldr		R2, [R1, #GPIO_ODR]			; do same process as above with ODR register
		orr		R3, R0, R2
		str		R3, [R1, #GPIO_ODR]

		ldr		R2, [R1, #GPIO_AFSEL]		; do same process as above with AFSEL register
		bic		R3, R0, R2
		str		R3, [R1, #GPIO_AFSEL]

		ldr		R2, [R1, #GPIO_DEN]			; do same process as above with DEN register
		orr		R3, R0, R2
		str		R3, [R1, #GPIO_DEN]

;; for col

		ldrb	R0, [R4, #PIN0]				; load each pin into R0-R3 to be or
		ldrb	R1, [R4, #PIN1]				; remains PIN0-PIN3 because of address offset
		ldrb	R2, [R4, #PIN2]
		ldrb	R3,	[R4, #PIN3]

		orr 	R0, R1
		orr 	R0, R2
		orr 	R0, R3

		ldr		R1, [R4, #BASE]
		ldr		R2, [R1, #GPIO_DIR]			; get value stored at GPIO_DIR and load into R2
		bic		R3, R0, R2					; orr value in GPIO_DIR and bitmask (R0) and store in R3
		str 	R3, [R1, #GPIO_DIR]			; store back out to GPIO_DIR

		ldr		R2, [R1, #GPIO_PUR]			; do same process as above with PUR register
		orr		R3, R0, R2
		str 	R3, [R1, #GPIO_PUR]

		ldr		R2, [R1, #GPIO_ODR]			; do same process as above with ODR register
		bic		R3, R0, R2
		str		R3, [R1, #GPIO_ODR]

		ldr		R2, [R1, #GPIO_AFSEL]		; do same process as above with AFSEL register
		bic		R3, R0, R2
		str		R3, [R1, #GPIO_AFSEL]

		ldr		R2, [R1, #GPIO_DEN]			; do same process as above with DEN register
		orr		R3, R0, R2
		str		R3, [R1, #GPIO_DEN]



		mov		R0, #0
		bl 		setrow

		pop 	{PC, R4-R12}

        .endasmfunc

;; bool getKey( uint8_t *col, uint8_t *row );

        .align  4                       	; force word alignment
getKey: .asmfunc

		push 	{LR, R4-R12}

		mov		R4, R0						; Move R0 (col) into R4
		mov 	R5, R1						; Move R1 (row) into R5

		ldr		R6, CPORT_ADDR				; move address of cport into R6 register
		ldr		R7, RPORT_ADDR				; move address of rport into R7 register

		str 	R0, [R6]					; put contents of *col into CPORT at the address
		str		R1, [R7]					; put contents of *row into RPORT at the address

		ldrb	R0, [R4, #PIN0]				; load each pin of col into R0-R3 to be or'ed
		ldrb	R1, [R4, #PIN1]
		ldrb	R2, [R4, #PIN2]
		ldrb	R3,	[R4, #PIN3]

		orr		R0, R1						; pins all high
		orr 	R0, R2
		orr 	R0, R3

		ldr 	R1, [R4, #BASE]

		pop		{PC, R4-R12}
        .endasmfunc

        .data                           ; switch to data (RAM) section
        .align  4                       ; force word alignment

;; Place any variables stored in RAM here

        .end
