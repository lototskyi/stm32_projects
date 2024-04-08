



			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.global		__main


__main:
			mov r0,#0x11
			lsl r1,r0,#1	//shift 1 bit left = 17*2^1 = 34
			lsl r2,r1,#1	//shift 1 bit left = 34*2^1 = 68


stop:
			b	stop


			.align
			.end
