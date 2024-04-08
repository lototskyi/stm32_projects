

.equ RCC_BASE, 				0x40023800
.equ AHB1ENR_OFFSET,		0x30
.equ RCC_AHB1ENR,	 		(RCC_BASE + AHB1ENR_OFFSET)

.equ GPIOD_BASE,			0x40020C00
.equ MODER_OFFSET,			0x00
.equ GPIOD_MODER,			(GPIOD_BASE + MODER_OFFSET)
.equ ODR_OFFSET,			0x14
.equ GPIOD_ODR,				(GPIOD_BASE + ODR_OFFSET)

.equ BSRR_OFFSET,			0x18
.equ GPIOD_BSRR,			(GPIOD_BASE + BSRR_OFFSET)

.equ GPIOA_BASE,			0x40020000
.equ GPIOA_MODER,			(GPIOA_BASE + MODER_OFFSET)
.equ GPIOA_BSRR,			(GPIOA_BASE + BSRR_OFFSET)

.equ IDR_OFFSET,			0x10
.equ GPIOA_IDR,				(GPIOA_BASE + IDR_OFFSET)

.equ GPIOD_EN,				(1 << 3)
.equ MODER_12_OUT,			(1 << 24)
.equ LED_ON,				(1 << 12)
.equ LED_OFF,				(1 << 0)
.equ ONESEC,				5333333
.equ GPIOA_EN,				(1 << 0)

.equ BSRR_12_SET,			(1 << 12)
.equ BSRR_12_RESET,			(1 << 28)

//Active low switch
.equ BTN_ON,				0x0001
.equ BTN_OFF,				0x0000
.equ BTN_PIN,				0x0001

			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.global		__main


__main:

			bl	gpio_init

loop:
			bl	get_input
			cmp r0,#BTN_ON
			beq	turn_led_on
			cmp r0,#BTN_OFF
			beq	turn_led_off

			b	loop


turn_led_on:
			mov	r1,#0
			ldr	r2,=GPIOD_BSRR
			mov	r1,#BSRR_12_SET
			str	r1,[r2]
			b	loop

turn_led_off:
			mov	r1,#0
			ldr	r2,=GPIOD_BSRR
			mov	r1,#BSRR_12_RESET
			str	r1,[r2]
			b	loop

get_input:
			ldr	r1,=GPIOA_IDR
			ldr	r0,[r1]
			and	r0,r0,#BTN_PIN
			bx	lr

gpio_init:
			/*Set PD12 as output*/
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

			/*Enable clock access to GPIOA*/
			ldr r0,=RCC_AHB1ENR
			ldr r1,[r0]
			orr r1,#GPIOA_EN
			str r1,[r0]
			bx	lr

stop:
			b	stop

			.align
			.end
