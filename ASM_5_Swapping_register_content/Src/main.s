
			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.global		__main


__main:
			ldr	r0,=0xBABEFACE
			ldr	r1,=0xDEADBEEF

			eor	r0,r0,r1
			eor	r1,r0,r1
			eor	r0,r0,r1
stop:
			b	stop


			.align
			.end
