;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida
; CDA 3331C - Microprocessors
;
; This Function creates an unsorted array of 16-bit integers. It then copies that array to a
; 2nd array. This 2nd array is then sorted in ascending order. The 2nd array is then iterated and
; all the duplicate entries are copied into a 3rd array.
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
CLR_REG     clr     R5                      ; R5 interates the array
            clr     R6                      ; R6 stores the minimum value
            clr     R10                     ; R10 is the array index counter

            mov.w   #6, R10                 ; copy # array indices to R10
            mov.w   #202h, R5               ; put address of 1st array index into R5

INIT_A      mov.w   #6, &200h               ; 1st array value = number of indices (6)
            mov.w   #15, &202h              ; index 1: add values to the array
            mov.w   #9, &204h               ; index 2
            mov.w   #20, &206h              ; index 3
            mov.w   #1, &208h               ; index 4
            mov.w   #25, &20Ah              ; index 5
            mov.w   #10, &20Ch              ; index 6 <-- last value

FIND_MIN    mov.w   @R5+, R6                ; 1st array index == # of elements
            dec.w   R10                     ; decrement counter to n-1 elements

LOOP        cmp.w   @R5+, R6                ; compare next value with R6 contents
            jl      SKIP                    ; skip next step if value is less
            mov.w   -2(R5), R6              ; move the lesser value to R6

SKIP        dec.w   R10                     ; decrement the counter
            jnz     LOOP                    ; keep looping to the end of array

DONE        jmp     DONE                    ; jump in place forever

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
            
