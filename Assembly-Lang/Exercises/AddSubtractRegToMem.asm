;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida 
; CDA 3331C - Microprocessors
;
; This is a simple program for a lab experiment which takes 4 values from 4 
; different registers (R4, R5, R6, R10). It adds R4, R5, and R6 together and
; puts the result in R7. Then, the value in R10 is subtracted from the total 
; in R7. All these values in R4, R5, R6, R7, and R10 are then written to
; consecutive memory blocks.
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
Setup		clr.w	R4					; clear all regiter values
			clr.w	R5
			clr.w	R6
			clr.w	R7
			clr.w	R10

			clr.w	&0200h				; clear needed memory locations
			clr.w	&0202h
			clr.w	&0204h
			clr.w	&0206h
			clr.w	&0208h

			mov.w	#04, R4				; copy 04(dec) to Register R4
			mov.w	#03, R5				; copy 03(dec) to Register R5
			mov.w	#10, R6				; copy 10(dec) to Register R6
			mov.w	#15, R10			; copy 15(dec) to Register R10

Addition	mov.w	R4, R7				; copy value from R4 to R7
			add.w	R5, R7				; add value from R5 to R7
			add.w	R6, R7				; add value from R6 to R7

Subtraction	sub.w	R10, R7				; subtract value of R10 from R7

Store		mov.w   R4, &0200h			; copy content of R4 to Mem 0x0200
			mov.w	R5, &0202h			; copy content of R5 to Mem 0x0202
			mov.w	R6, &0204h			; copy content of R6 to Mem 0x0204
			mov.w	R10, &0206h			; copy content of R10 to Mem 0x0206
			mov.w	R7, &0208h			; copy content of R7 to Mem 0x0208

Mainloop	jmp     Mainloop          	; Infinite Loop
;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
