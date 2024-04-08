//(A + 8B + 7C -27)/4
//Let A = 25, B = 19, C = 99


			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.global		__main


__main:
			mov r0,#25
			mov r1,#19

			//r0 = A + 8B
			add r0,r0,r1,lsl #3

			mov r1,#99
			mov r2,#7

			//add 7C to r0 and put result in r0
			mla r0,r1,r2,r0
			sub r0,r0,#27

			//divide the total by 4
			mov r0,r0,asr #2

stop:
			b	stop


			.align
			.end
