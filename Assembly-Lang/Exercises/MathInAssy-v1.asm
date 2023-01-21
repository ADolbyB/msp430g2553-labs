;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida
; CDA-3331C: Intro to Microprocessors
;
; This program executes Summation, factorial, multiplication/division, absolute 
; value operations as part of a 
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
;  MAIN PROGRAM IS HERE:
Lab3		clr.w	R4						; Contains original User Input
			clr.w	R5						; Used as a counter to decrement for 'n!' calculation
			clr.w	R6						; contains the value of 'n!'
			clr.w	R7						; Used for FCALC
			clr.w	R8						; Holds Value of 'X' - Summation value
			clr.w	R10						; R10 used for factorial to decrement 'n'
			mov.w 	#-7, R4					; PROGRAM USER INPUT VALUE "a" Stored in R4

XCALC		mov.w	R4, R5					; BACKUP R4 to R5
			call 	#ABSOL					; compute 'abs(a)' in R5
			mov.w	R5, R10					; R10 decrements as a counter for factorial / summation function

SUMMATION	mov.w 	R5, R6					; starting the upper limit with value "A" of the summation in R6
			call 	#nFACT					; get the factorial value of R6 and save in R6
											; here: R6 = n! & R5 = 0

			mov.w	R10, R5					; copy (n - 1) to R5 for (n - 1)! calculation
			call	#DOUBLE					; subroutine to double R6 and add to R8
			dec		R5						; decrement R5 value by 1 to compute next factorial for summation
			jz		SAVE_R5					; End summation calculation of 'X' when R5 = 0
			jnz		SUMMATION				; perform summation until R5 = 0

SAVE_R5		add.w	#2, R8					; ADD 2(0!) to the value of 'X' at the end
			mov.w	R8, R5					; save the value 'X' results to R5

FCALC		mov.w 	R5, R7					; start F calculation
			rla.w	R7						; multiply by 2
			rla.w	R7						; multiply by 2 again
			add.w	#40, R7					; add 40 to the result
			rra.w	R7						; divide result by 2

ENDLAB3		jmp 	ENDLAB3					; jump in place forever


; Absolute subroutine take a value form R5 and converts it to its absolute value

ABSOL:		tst 	R5						; TEST if R5 is negative
			jn		twoscompl				; jump if negative to perform 2's complement
			ret								; end subroutine if number is poitive

twoscompl	inv 	R5						; invert the bits of R5 (1's complement)
			inc		R5						; add 1 bit (2's complement)

ABSOLend	ret								; end subroutine


; Factorial subroutine takes number n from R6 and computes n! to R6

nFACT:		mov.w	R10, R5					; use original value in R4 to calculate n! factorial
			tst.w	R5						; test if R5 is negative (Input for calcuation)
			jn		NEG						; skip if user input R5 < 0
			jz		ZERO					; skip if user input R5 == 0
			mov.w	R5, R6					; move "n" to R6

LOOP		dec.w	R5						; decrement n to n-1
			jz		nFACTend				; end function if counter is ZERO
			call	#MULT					; call multiply function
			jmp		LOOP					; loop back to decrement R5

NEG			clr.w	R6						; clear R6 to 0000 (n! of negative = 0)
			jmp		nFACTend				; end subroutine

ZERO		mov.w	#1, R6					; set R6 to 1 (n! of 0 = 1)
			jmp		nFACTend				; end subroutine

nFACTend	ret								; end subroutine

; The multiplier subroutine based on shift and add
; It takes R5 as the multiplier and R6 as the multiplicand
; to avoid multiplication overflow, both R5 and R6 should be limited to one byte and thus ANDed with 0x00FF.
; but due to factorial calculation, we will not mask R6 to let it grow beyond 255

MULT:		push.w	R5						; save current Register values to the stack
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

MULTend		ret								; END subroutine


; DOUBLE Subroutine: Takes the n! factorial result, doubles it and adds to R8 value

DOUBLE		rla.w	R6						; Shift Bits (w/o carry) of R8 left by 1 to multiply by 2
			add.w	R6, R8					; add doubled value to R8 for accumulation
			dec.w	R10						; decrement R10 counter for factorial
			ret								; exit subroutine

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
            
