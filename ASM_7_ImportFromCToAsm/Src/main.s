



			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.global		__main
			.globl		num
			.globl 	Adder


__main:
			ldr r1,=num
			mov r0,#100
			str r0,[r1]
			bl Adder

stop:
			b	stop


			.align
			.end
