;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida
; CDA 3331C - Microprocessors
; March 17, 2021
;
; Sequence Detector VERSION 2
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
; In this program, a simulated continuous stream of bits is input to the microcontroller.
; As soon as the value "1010xx1010" is found in the register as the stream of data is input, The program sets R5
; to "1111111111111111" aka 0xFFFF and then ends. This is an alternate version of the Sequence Detector v1.0
;
START       mov.w   #0xA28A, R5             ; examine some random value in R5

SEQ         push    R6                      ; save values in registers before routine starts
            push    R10
            push    R11

            mov.b   #7, R10                 ; store # of iterations to RRC.W R5 (16 - 10 + 1)

LOOP        mov.w   R5, R6                  ; save R5 contents in R6 to be MASKED
            and.w   #0x03CF, R6             ; perform MASKING of the string pattern
            cmp.w   #0x028A, R6             ; SEARCH for String "1010xx1010" = 0x028A
            jnz     SKIP                    ; SKIP if no match

SET_F       mov.w   #0xFFFF, R5             ; set R5 to 0xFFFF if pattern is found in R6
            jmp     END                     ; END the subroutine if found

SKIP        rrc.w   R5                      ; Rotate Right the contents of R5 to be searched
            dec.b   R10                     ; decrement counter
            jnz     LOOP                    ; jump if not at end of search string

CLEAR       clr     R5                      ; clear R5 if pattern not found

END         pop     R11                     ; restore original values of registers
            pop     R10
            pop     R6

            ret                             ; return from subroutine

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
            
