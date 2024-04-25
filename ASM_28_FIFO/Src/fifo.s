

			.section	.data

putPt:		.space		4 		/*Allocate 4 bytes of menory filled with zeros*/
getPt:		.space		4

.equ SIZE, 				8


fifo:		.space		SIZE	/*FIFO can accept size SIZE-1 items*/

.equ fifo_addr,			fifo
.equ fifo_end_addr,		fifo + SIZE
.equ putPt_addr,		putPt
.equ getPt_addr,		getPt

			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.globl	fifo_init
			.globl	fifo_put
			.globl	fifo_get
			.globl	fifo_size


fifo_init:
			/* Set putPt and getPt to fifo_addr */
			ldr	r0,=fifo_addr
			ldr	r1,=putPt_addr
			str	r0,[r1]

			ldr r1,=getPt_addr
			str	r0,[r1]

			bx	lr


/*
	R0 has data to be put into fifo
	R0 has return value, 1 - success and 0 - failure
*/

fifo_put:
			ldr		r1,=putPt_addr
			ldr		r2,[r1]
			add		r3,r2,#1
			ldr		r12,=fifo_end_addr
			cmp		r3,r12				/*Check if end of fifo is reached*/
			bne		_proceed_with_put
			ldr		r3,=fifo_addr		/*Wrap around, meaning set position to starting point*/

_proceed_with_put:
			ldr		r12,=getPt_addr
			ldr		r12,[r12]
			cmp		r3,r12
			bne 	_not_full
			mov		r0,#0				/*Return failure*/
			bx		lr

_not_full:
			strb	r0,[r2]
			str		r3,[r1]
			mov		r0,#1				/*Return success*/
			bx		lr

/*
	Takes address to store value from fifo as argument,
	destination address is passed to R0
	R0 has return value, 1 - success and 0 - failure
*/

fifo_get:
			push	{r4,r5,lr}
			ldr		r1,=putPt_addr
			ldr		r1,[r1]
			ldr		r2,=getPt_addr
			ldr		r3,[r2]
			cmp		r1,r3				/*check if fifo is empty (putPt_addr==getPt_addr means empty)*/
			bne		_fifo_not_empty

			mov		r0,#0				/*Fifo is empty, nothing to get*/
			b		_cleanup

_fifo_not_empty:
			ldrsb	r4,[r3]
			strb	r4,[r0]
			add		r3,r3,#1			/*move to next place*/
			ldr		r5,=fifo_end_addr
			cmp		r3,r5				/*check if getPt + 1 = end of fifo*/
			bne		_update_getPt

			ldr		r3,=fifo_addr

_update_getPt:
			str		r3,[r2]

_cleanup:
			pop		{r4,r5,lr}

/*Returns current number of elements in fifo
R0 contains return value*/
fifo_size:
			ldr		r1,=putPt_addr
			ldr		r1,[r1]
			ldr		r2,=getPt_addr
			ldr		r3,[r2]
			sub		r0,r1,r3
			and		r0,#(SIZE-1)
			bx		lr

			.align
			.end

