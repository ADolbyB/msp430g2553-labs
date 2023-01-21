;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
; Joel Brigida 
; CDA 3331C - Microprocessors
; Lab Final - 4/27/21
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
LINE_A		clr.w	SR					; clear SR to 0000
			clr.w	R5					; clear R5 register
			clr.w	R6					; clear R6 register
			clr.w	&0x0200				; clear contents of 0x0200h
			clr.w	&0x0202				; clear contents of 0x0202h
			clr.w 	&0x0204				; clear contents of 0x0204h
			clr.w	&0x0206				; clear contents of 0x0206h
			clr.w	&0x0208				; clear contents of 0x0208h
			clr.w	&0x020A				; clear contents of 0x020Ah

			mov.w	#0x0102, SR			; Set N and V bits in SR to 1
			mov.w	#0x1234, R5			; copy #0x1234 to R5
			mov.w	#0xA006, R6			; copy #0xA006 to R6

     	 	mov.w	#0x5800, &0x0200	; copy #0x5800 to &0x0200
			mov.w	#0x5700, &0x0202	; copy #0x5700 to &0x0202
			mov.w	#0x5600, &0x0204	; copy #0x5600 to &0x0204
			mov.w	#0x5500, &0x0206	; copy #0x5500 to &0x0206

Final_1		mov.w	#0x9599, R5			; copy #0x5995 to R5
            mov.w	#0x0206, R6			; copy #0x0202 to R6
            mov.b	R5, 0(R6)			; move a byte from R5 to &->(R6)

LINE_B      clr.w	SR					; clear SR to 0000
			clr.w	R5					; clear R5 register
			clr.w	R6					; clear R6 register
			clr.w	&0200h				; clear contents of 0x0200h
			clr.w	&0202h				; clear contents of 0x0202h
			clr.w 	&0204h				; clear contents of 0x0204h
			clr.w	&0206h				; clear contents of 0x0206h

			mov.w	#0x0104, SR			; set N and V bits to 1
			mov.w	#0x0204, R5			; copy #0x0204 to R5
			mov.w	#0x0208, R6			; copy #0x0208 to R6

			mov.w	#0x8800, &0x0200	; copy #0x8800 to &0x0200
			mov.w	#0x8700, &0x0202	; copy #0x5700 to &0x0202
			mov.w	#0x8600, &0x0204	; copy #0x5600 to &0x0204
			mov.w	#0x8500, &0x0206	; copy #0x5500 to &0x0206

Final_2		mov.w	@R5+, -2(R6)		; copy &->(R5) to &->(R6 - 2)
			bis.b	#0x0004, R6			; bit set a byte (logic OR)
			inv.b	-6(R5)				; invert a byte at &->(R5 - 6)

LINE_C      clr.w	SR					; clear SR to 0000
			clr.w	R5					; clear R5 register
			clr.w	R6					; clear R6 register
			clr.w	&0200h				; clear contents of 0x0200h
			clr.w	&0202h				; clear contents of 0x0202h
			clr.w 	&0204h				; clear contents of 0x0204h
			clr.w	&0206h				; clear contents of 0x0206h

			mov.w	#0x0101, SR			; set V and C bits to 1
			mov.w	#0x0101, R5			; copy #0x0101 to R5
			mov.w	#0x0206, R6			; copy #0x0202 to R6

			mov.w	#0x9808, &0x0200	; copy number 0x5800 to &0x0200
			mov.w	#0x9707, &0x0202	; copy number 0x5700 to &0x0202
			mov.w	#0x9606, &0x0204	; copy number 0x5600 to &0x0204
			mov.w	#0x9505, &0x0206	; copy number 0x5500 to &0x0206

Final_3		addc.w	R5, R6				; add w/ carry R5 to R6
			incd.w 	R6					; increment R6 by 2
			rrc.b	&0x204				; Rotate Right w/ carry one byte at &0x0200

Mainloop	jmp     Mainloop        	; Jump in place forever / infinite loop

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
            
