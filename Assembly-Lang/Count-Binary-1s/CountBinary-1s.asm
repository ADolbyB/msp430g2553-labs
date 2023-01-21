;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida 
; CDA 3331C - Microprocessors
; 
; This program takes a 16-bit word from R5 and uses the carry digit to detect
; a binary '1' as it rotates the value out to the right. If a 1 is detected.
; The counter (R6) is incremented. The program terminates when all binary 1s
; are rotated out of R5 and it only contains 0s. The total number of 1s in the
; binary string will be stored in R6 when finished.
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
			clr.w 	R5						; clear out R5 to 0000
			mov.w	#0xEDCB, R5				; set value of R5

COUNT		clr.w	R6

LOOP		tst 	R5						; check value in R5
			jeq		DONE					; jump if equal to 'DONE'

			rla.w	R5						; rotate left add: R5 value
            jnc		SKIP					; jump if no carry to 'SKIP'
            inc.b	R6						; increment counter in R6

SKIP		jmp		LOOP					; keep looping

DONE		jmp		DONE					; jump in place forever
			;ret as a subroutine

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
            
