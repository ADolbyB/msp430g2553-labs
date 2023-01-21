;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida
; CDA 3331C - Microprocessors
; March 17, 2021
;
; Sequence Detector WORDS
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
; Here, we are searching for a 9 bit string "101xxx101" = 0x0145 (9 bits) in the contents of R5
;
START		mov.w	#0xA28A, R5				;String to be searched: 1010 0010 1000 1010
			mov.w	#0xA28A, R8				;Preserve original Register Value

SEQ_DET		mov.b	#8, R10					;store # of iterations to RRC.W R5
			clr		R11						;zero out counter for # of string matches

LOOP		mov.w	R5, R6					;save R5 contents in R6 to be MASKED
			and.w	#0x01C7, R6				;perform MASKING of the string pattern
			cmp.w	#0x0145, R6				;SEARCH for String "101xxx101" = 0x0145 in R6
			jnz		SKIP					;SKIP if no match
			inc.b	R11						;increment counter if there is a match

SKIP		rrc.w	R5						;Rotate Right the contents of R5 to be searched
			dec.b	R10						;decrement counter
			jnz		LOOP					;jump if not at end of search string

			mov.w	R8, R5					;Restore original value back to R5
END			jmp		END						;jump in place forever
                                            

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
            
