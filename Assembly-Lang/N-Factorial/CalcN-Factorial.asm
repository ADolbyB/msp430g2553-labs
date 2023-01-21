;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida 
; CDA 3331C - Microprocessors
;
; This is a model subroutine for computing n! (n factorial)
; The value of n is input into R5, and the result n! is placed in R6
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
;
			clr		R5						; clear registers
			clr		R6						;
			clr		R7						;
			clr		R8						;
			mov.w	#6, R5					; INPUT R5: 5! = 120dec / 0x078, 6! = 720dec / 0x2D0

FACTORIAL	tst.w	R5						; test if R5 is negative
			jn		NEG						; jump to NEG if R5 is negative
			jz		ZERO					; jump to ZERO if R5 is Zero
			mov.w	R5, R6					; move 'n' to R6 (R6 holds factorial result)

LOOP		dec.w	R5						; decrement 'n' to 'n-1'
			jz		END						; end function when R5 gets to zero

			call	#MULT					; call MULT to perform multiplication
			jmp		LOOP					; after multiplication: decrement R5

NEG			clr.w	R6						; if INPUT R5 < 0, n! = 0
			jmp		END						; end function if input = negative

ZERO		mov.w	#1, R6					; if INPUT R5 = 0, n! = 1
			jmp		END						; end the subroutine

END			jmp		END						; jump in place forever (End of function)


; Multiplication Subroutine

MULT		push.w	R5						; save current Register values to the stack
			push.w	R7						;
			push.w 	R8						;
			mov.w	#8, R8					; 8 bit multiplication, so we loop 8 times

			clr.w	R7						; additive accumulator should start with zero
			and.w	#0x00FF, R5				; clear upper 8 bits of multiplier
			;and.w 	#0x00FF, R6				; clear upper 8 bits of multiplicand (optional)

nextbit		rrc.w	R5						; shift multiplier bits one at a time to the carry
			jnc		twice					; if no carry skip the add

addmore		add.w	R6, R7					; add a copy of the multiplicand to the accumulator

twice		add.w	R6, R6					; multiplicand times 2, (shifted 1 bit left)
			dec.w 	R8						; dec loop counter
			jnz		nextbit					; jump to check next bit of the multiplier
			mov.w 	R7, R6					; save the result in R6

			pop.w	R8						; restore prev values to Registers
			pop.w	R7						;
			pop.w 	R5						;

MULTend		ret								; return from subroutine when finished

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
            
