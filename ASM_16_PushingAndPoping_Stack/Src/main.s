

			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.global		__main


__main:
			ldr r3,=0xDEADBEEF
			ldr r4,=0xBABEFACE

			push {r3}
			push {r4}

			pop {r5}
			pop {r6}

stop:
			b	stop


			.align
			.end
