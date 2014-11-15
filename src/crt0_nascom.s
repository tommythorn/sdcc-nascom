;; copied from http://www.cpcmania.com/Docs/Programming/Introduction_to_programming_in_SDCC_Compiling_and_testing_a_Hello_World.htm
;; FILE: crt0.s
;; Generic crt0.s for a Z80
;; From SDCC..
;; Modified to suit execution on the Amstrad CPC!
;; by H. Hansen 2003
;; Original lines has been marked out!

	.module	crt0
	.globl	_main

	.area	_HEADER (ABS)
;; Reset vector
	.org	0x1000 ;; Start from address #1000
	jp	init

        .area   _CODE
init:

;; Stack just below the ROM BASIC
	ld	sp,#0xE000

;; Initialise global variables
	call	gsinit
	call	_main
	jp	_exit

	;; Ordering of segments for the linker.
	.area	_HOME
	.area	_CODE
	.area	_GSINIT
	.area	_GSFINAL

	.area	_DATA
	.area	_BSS
	.area	_HEAP

	.area	_CODE
__clock::
	ret

_exit::
	rst	0x18            ; SCAL MRET
        .db	0x5B
	halt			; Shouldn't even get here

_getchar::
_getchar_rr_s::
_getchar_rr_dbs::
        rst	0x18            ; SCAL BLINK
        .db	0x7B
	ld	h,#0
	ld	l,a
;;; rewrite \r to \n
        sub     a, #0x0D
        jr      NZ,00103$
        ld	l,#0x0A
00103$:
	ret

_putchar::
_putchar_rr_s::
	ld	hl,#2
	add	hl,sp
	ld	e,(hl)

_putchar_rr_dbs::
        ;; if (e == '#') e = 0x9E
	ld	a,e
        sub     a, #0x23        ; '#'
        jr      NZ,00104$
        ld	e, #0x9E
00104$:
        ;; if (e == '\n') rst30('\r'), e = '\n';
	ld	a,e
        sub     a, #0x0A
        jr      NZ,00102$

	ld	a,#0x0D
  	rst	0x30            ; ROUT
	ld	e,#0x0A

00102$:
        ;; rst30(e)
	ld	a,e
  	rst	0x30
	ret

	.area	_GSINIT
gsinit::
	ld	bc, #l__INITIALIZER
	ld	a, b
	or	a, c
	jr	Z, gsinit_next
	ld	de, #s__INITIALIZED
	ld	hl, #s__INITIALIZER
	ldir
gsinit_next:

	.area	_GSFINAL
	ret
