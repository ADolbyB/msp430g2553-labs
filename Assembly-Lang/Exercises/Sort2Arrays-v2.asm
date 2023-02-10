;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida 
; CDA 3331C - Microprocessors
;
; Sort Algo #2:
; This function takes 2 unsorted arrays and sorts them both into 2 new
; sorted arrays
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"	; Include device header file
            
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
; Memory allocation of arrays must be done prior to the RESET & StopWDT

ARY1        .set    0x0200                  ; Memory allocation ARRAY_1
ARY1S       .set    0x0210                  ; Memory allocation ARRAY_1_SORTED
ARY2        .set    0x0220                  ; Memory allocation ARRAY_2
ARY2S       .set    0x0230                  ; Memory allocation ARRRY_2_SORTED

RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer

;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
;
REG         clr     R4                      ; Clear R4: points to UNSORTED array 1st index
            clr     R5                      ; Clear R5: iterates UNSORTED array
            clr     R6                      ; Clear R6: points to SORTED array 1st index
            clr     R7                      ; Clear R7: iterates SORTED array
            clr     R9                      ; INNER loop counter
            clr     R10                     ; Clear R10: used as index counter
            clr     R11                     ; Backs up starting array index
            clr     R12                     ; swap backup for larger value


SORT1       mov.w   #ARY1, R4               ; Initialize R4 to point to ARY1 in the memory
            mov.w   #ARY1S, R6              ; Initialize R6 to point to ARY1S in the memory where the sorted ARY1 will be stored
            call    #ArraySetup1            ; Create elements are store them in ARY1
            call    #COPY                   ; Copy the elements from the ARY1 space to ARY1S space
            mov.w   #0x201, R5              ; set R5 to point to beginning of UNSORT array
            mov.w   #0x211, R7              ; set R7 to point to beginning of SORT array
            mov.w   R7, R11                 ; backup the starting array index to R11
            mov.w   #10, R10                ; set array size to R10 as a counter
            mov.w   #1, R9                  ; set inner loop counter to start at 1
            call    #SORT                   ; Calling Subroutine Sort with parameter passing in R4 abd R6

SORT2       mov.w   #ARY2, R4               ; Initialize R4 to point to ARY2 in the memory
            mov.w   #ARY2S, R6              ; Initialize R6 to point to ARY2S in the memory where the sorted ARY2 will be stored
            call    #ArraySetup2            ; Create elements are store them in ARY2
            call    #COPY                   ; Copy the elements from the ARY2 space to ARY1S space
            mov.w   #0x0221, R5             ; reset R5 to beginning of 3rd UNSORT array
            mov.w   #0x0231, R7             ; reset R7 to beggining of 4th SORT array
            mov.w   R7, R11                 ; backup the starting array index to R11
            mov.w   #10, R10                ; set array size as counter
            mov.w   #1, R9                  ; set inner loop counter to start at 1
            call    #SORT                   ; Calling Subroutine Sort with parameter passing in R4 abc R6

Mainloop    jmp     Mainloop                ; loop in place forever

; Array element initialization Subroutine
ArraySetup1 mov.b   #10, 0(R4)              ; Define 10 elements in the array
            mov.b   #33, 1(R4)              ; index 1: store elements 1 - 10
            mov.b   #-91, 2(R4)             ; index 2
            mov.b   #-75, 3(R4)             ; index 3
            mov.b   #82, 4(R4)              ; index 4
            mov.b   #11, 5(R4)              ; index 5
            mov.b   #-28, 6(R4)             ; index 6
            mov.b   #-99, 7(R4)             ; index 7
            mov.b   #31, 8(R4)              ; index 8
            mov.b   #-92, 9(R4)             ; index 9
            mov.b   #80, 10(R4)             ; index 10

            ret                             ; RETURN from subroutine

; Array element initialization Subroutine
ArraySetup2 mov.b   #10, 0(R4)              ; Define 10 elements in the array
            mov.b   #21, 1(R4)              ; index 1: store elements 1 - 10
            mov.b   #22, 2(R4)              ; index 2
            mov.b   #20, 3(R4)              ; index 3
            mov.b   #-49, 4(R4)             ; index 4
            mov.b   #-80, 5(R4)             ; index 5
            mov.b   #32, 6(R4)              ; index 6
            mov.b   #62, 7(R4)              ; index 7
            mov.b   #60, 8(R4)              ; index 8
            mov.b   #61, 9(R4)              ; index 9
            mov.b   #-82, 10(R4)            ; index 10

            ret                             ; RETURN from subroutine

; Copy original Array to allocated Array-Sorted space
COPY        mov.b   0(R4), R10              ; save n (# of elements) in R10 (counter)
            inc.b   R10                     ; increment by 1 to account for the byte n to be copied as well
            mov.w   R4, R5                  ; copy R4 to R5 so we keep R4 unchanged for later use
            mov.w   R6, R7                  ; copy R6 to R7 so we keep R6 unchanged for later use

LP          mov.b   @R5+, 0(R7)             ; copy elements using R5/R7 as pointers
            inc.w   R7                      ; increment R7 to next array index
            dec     R10                     ; decrement index counter
            jnz     LP                      ; jump if not zero to: LP (Keep looping)

            ret                             ; RETURN from subroutine

; BEGIN SORT Subroutine to Sort Array indices to new SORTED array

SORT        cmp.b   @R7, 1(R7)              ; compare index n with index n+1 and
IF_GRTR     jge     SKIP                    ; if: already in order: SKIP

INNER_LP    mov.b   0(R7), R12              ; Store larger value to R12
            mov.b   1(R7), 0(R7)            ; Store smaller value to lower array index
            mov.b   R12, 1(R7)              ; Store larger value to higher array index
            inc     R7                      ; increment *&address in R7 to next SORTED index
            inc.b   R9                      ; increment INNER_LP counter
            cmp.b   R9, R10                 ; end of array: R9 == R10
            jeq     OUTER_LP                ; decrement OUTER_LP counter if at end of array
            jmp     SORT                    ; continue sorting until the end

SKIP        inc     R7                      ; increment *&address in R7 to next SORTED index
            cmp.b   R9, R10                 ; end of array: R9 == R10
            jeq     OUTER_LP                ; decrement OUTER_LP counter if at end of array
            inc.b   R9                      ; increment INNER_LP counter
            jmp     SORT                    ; continue sorting until the end

OUTER_LP    dec.b   R10                     ; decrement OUTER_LP counter R10
            jz      DONE                    ; end if counter is ZERO
            mov.b   #1, R9                  ; RESET INNER_LP R9 counter to 0 again
            mov.w   R11, R7                 ; point R7 to beginning of array again
            jmp     SORT                    ; jump back to continue SORT FUNCTION

DONE        ret                             ; DONE sorting
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
            
