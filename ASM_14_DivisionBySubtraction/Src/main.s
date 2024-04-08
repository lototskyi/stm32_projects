

			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.global		__main


__main:
			ldr r0,=60
			mov r1,#10
			mov r2,#0

again:		cmp r0,r1

			//branch to stop if r0 is lower than r1

			blo stop
			sub r0,r0,r1
			add r2,r2,#1
			b again

stop:
			b	stop


			.align
			.end
