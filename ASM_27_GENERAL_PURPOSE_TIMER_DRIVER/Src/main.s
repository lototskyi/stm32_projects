

.equ RCC_BASE, 				0x40023800
.equ AHB1ENR_OFFSET,		0x30
.equ RCC_AHB1ENR,	 		(RCC_BASE + AHB1ENR_OFFSET)

.equ APB1ENR_OFFSET,		0x40
.equ RCC_APB1ENR,	 		(RCC_BASE + APB1ENR_OFFSET)

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

.equ TIM2_BASE,				0x40000000

.equ TIM_PSC_OFFSET,		0x28
.equ TIM2_PSC,				(TIM2_BASE + TIM_PSC_OFFSET)

.equ TIM_ARR_OFFSET,		0x2C
.equ TIM2_ARR,				(TIM2_BASE + TIM_ARR_OFFSET)

.equ TIM_CNT_OFFSET,		0x24
.equ TIM2_CNT,				(TIM2_BASE + TIM_CNT_OFFSET)
.equ TIM_CR1_OFFSET,		0x00
.equ TIM2_CR1,				(TIM2_BASE + TIM_CR1_OFFSET)

.equ TIM_SR_OFFSET,			0x10
.equ TIM2_SR,				(TIM2_BASE + TIM_SR_OFFSET)

.equ TIM2_EN,				(1 << 0)
.equ TIM_CR1_EN,			(1 << 0)
.equ TIM_SR_UIF,			(1 << 0)
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

			bl		gpio_init
			bl		tim2_1hz_init
			bl		led_blink


tim2_1hz_init:
			/*1. Enable clock access to TIM2*/
			ldr		r0,=RCC_APB1ENR
			ldr		r1,[r0]
			orr		r1,#TIM2_EN
			str		r1,[r0]

			/*2. Set prescaller value*/
			ldr		r0,=TIM2_PSC
			mov		r1,#(1600 - 1)	/*16 000 000 / 1600 = 10 000*/
			str		r1,[r0]

			/*3. Set the auto-reload value*/
			ldr		r0,=TIM2_ARR
			mov		r1,#(10000-1)	/* divide 10000 / 10000*/
			str		r1,[r0]

			/*4. Clear timer counter*/
			ldr		r0,=TIM2_CNT
			mov		r1,#0
			str		r1,[r0]

			/*5. Enable the timer*/
			ldr		r0,=TIM2_CR1
			mov		r1,#TIM_CR1_EN
			str		r1,[r0]

			bx		lr

__wait:
			/*Wait gor UIF flag to be set*/
			ldr		r1,=TIM2_SR
lp1:

			ldr		r2,[r1]
			and		r2,#TIM_SR_UIF
			cmp		r2,#0x00
			beq		lp1

			/*Clear UIF*/
			ldr		r3,[r1]
			bic		r3,r3,#TIM_SR_UIF
			str		r3,[r1]
			bx		lr

led_blink:
			ldr		r4,=GPIOD_BSRR
			mov		r1,#BSRR_12_SET
			str		r1,[r4]
			bl		__wait

			ldr		r4,=GPIOD_BSRR
			mov		r1,#BSRR_12_RESET
			str		r1,[r4]
			bl		__wait

			bl		led_blink

gpio_init:
			ldr 	r0,=RCC_AHB1ENR
			ldr 	r1,[r0]
			orr 	r1,#GPIOD_EN
			str 	r1,[r0]

			/*Set PD12 as output*/
			ldr 	r0,=GPIOD_MODER
			ldr 	r1,[r0]
			orr 	r1,#MODER_12_OUT
			str 	r1,[r0]
			bx		lr

stop:
			b		stop

			.align
			.end
