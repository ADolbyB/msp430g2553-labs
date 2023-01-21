;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida
; CDA 3331C - Microprocessors
; March 17, 2021
;
; Sequence Detector VERSION 1
; This routine searches the BINARY STRING in R5 for a specific WORD sequence.
; by shifting the contents of R5 right one bit at a time (ignore carry)
; The # of iterations to rotate R5 are stored in R10
; The MASK is stored in R6 as a BYTE.
; The number of string matches are stored in R11
; # of times to rotate right R5 = (16 bits - #bits in search sequence + 1)
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
; Here, we are searching for a 10 bit string "1010xx1010" = 0x028A (10 bits) in the contents of R5
; In this case, as soon as the pattern is found twice, the program sets R5 to 0xFFFF. This version
; requires a counter to count the instances of "1010xx1010" in the stream.
;
START		mov.w	#0xA28A, R5				; examine this value in R5

SEQ			push	R6						; save values in registers before routine starts
			push	R10
			push	R11
			mov.b	#2, R11					; Upon 2 occurrences of 1010xx1010, SET R5 to 0xFFFF

 			mov.b	#7, R10					; store # of iterations to RRC.W R5 (16 - 10 + 1)

LOOP		mov.w	R5, R6					; save R5 contents in R6 to be MASKED
			and.w	#0x03CF, R6				; perform MASKING of the string pattern
			cmp.w	#0x028A, R6				; SEARCH for String "1010xx1010" = 0x028A
			jnz		SKIP					; SKIP if no match
			dec.b	R11						; decrement R11 if match is found
			jz		SET_F					; set R5 to 0xFFFF if pattern is found twice

SKIP		rrc.w	R5						; Rotate Right the contents of R5 to be searched
			dec.b	R10						; decrement counter
			jz		CLEAR					; Clear R5 if pattern not found
			jnz		LOOP					; jump if not at end of search string

CLEAR		clr		R5						; clear R5 if pattern not found
			jmp		END						; jump to end

SET_F		mov.w	#0xFFFF, R5				; set R5 to 0xFFFF if pattern is found twice

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
            
