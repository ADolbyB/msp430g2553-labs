;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
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
;
START		mov.w	#0xA28A, R5				; examine some random value in R5

SEQ			push	R6						; save values in registers before routine starts
			push	R10
			push	R11

 			mov.b	#6, R10					; store # of iterations to RRC.W R5 (16 - 11 + 1) = 6

LOOP		mov.w	R5, R6					; save R5 contents in R6 to be MASKED
			and.w	#0x07DF, R6				; perform MASKING of the string pattern: 0x07DF
			cmp.w	#0x0492, R6				; SEARCH for String "10010x10010" = 0x0492
			jnz		SKIP					; SKIP if no match

CLR_R5		clr		R5						; Clear R5 if pattern is found in R6
			jmp		END						; END the subroutine if found

SKIP		rrc.w	R5						; Rotate Right the contents of R5 to be searched
			dec.b	R10						; decrement counter
			jnz		LOOP					; jump if not at end of search string

SET_F		mov.w	#0xFFFF, R5				; set R5 to 0xFFFF if pattern is NOT found in R6

END			pop		R11						; restore original values of registers
			pop		R10
			pop		R6

			ret								; return from subroutine

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
            
