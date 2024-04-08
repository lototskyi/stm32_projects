



			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.globl	number_func



number_func:
			mov	r0,#121

			LSL	r4,r6,#4 //r4 = r6 << 4
			MOV	r3,#5
			LSL r4,r6,r3 //r4 = r6 << # specified in r3
			ROR r4,r6,#12 //r4 = r6 rotated right 12 bits, or r4 = r6 rotated left 20 bits (32-12)

			LSR r0,r2,#24
			ORR r3,r0,r3,LSL #8 //r3 is shifted up to 8, then ORed with r0 and insert into r3

			bx  lr

			.align
			.end
