

.equ RCC_BASE, 				0x40023800
.equ AHB1ENR_OFFSET,		0x30
.equ RCC_AHB1ENR,	 		(RCC_BASE + AHB1ENR_OFFSET)

.equ GPIOD_BASE,			0x40020C00
.equ GPIOD_MODER_OFFSET,	0x00
.equ GPIOD_MODER,			(GPIOD_BASE + GPIOD_MODER_OFFSET)
.equ GPIOD_ODR_OFFSET,		0x14
.equ GPIOD_ODR,				(GPIOD_BASE + GPIOD_ODR_OFFSET)

.equ GPIOD_EN,				(1 << 3)
.equ MODER_12_OUT,			(1 << 24)
.equ LED_ON,				(1 << 12)
.equ LED_OFF,				(1 << 0)
.equ ONESEC,				5333333

			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.global		__main


__main:
			/*Enable clock access to GPIOD*/
			//Load address of RCC_AHB1ENR into r0
			ldr r0,=RCC_AHB1ENR
			//load value at address found in r0 into r1
			ldr r1,[r0]
			orr r1,#GPIOD_EN
			//store content in r1 at address found in r1
			str r1,[r0]

			/*Set PD12 as output*/
			ldr r0,=GPIOD_MODER
			ldr r1,[r0]
			orr r1,#MODER_12_OUT
			str r1,[r0]

			mov r1,#0
			ldr r2,=GPIOD_ODR
Blink:
			mov r1,#LED_ON
			str r1,[r2]
			ldr r3,=ONESEC
			bl  Delay

			mov r1,#LED_OFF
			str r1,[r2]
			ldr r3,=ONESEC
			bl  Delay

			b Blink

Delay:
			subs r3,r3,#1
			//branch if Z flaf is not equal to zero
			bne  Delay
			bx   lr

			.align
			.end
