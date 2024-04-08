
.equ SYST_CSR,				0xE000E010
.equ SYST_RVR,				0xE000E014
.equ SYST_CVR,				0xE000E018

.equ SYSTICK_24BIT_MAX,		0x00FFFFFF
.equ SYST_CSR_EN,			(1 << 0)
.equ SYST_CSR_CLKSRC,		(1 << 2)
.equ SYST_CSR_CNTFLG,		(1 << 16)

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

.equ GPIOD_EN,				(1 << 3)
.equ MODER_12_OUT,			(1 << 24)
.equ LED_ON,				(1 << 12)
.equ LED_OFF,				(1 << 28)
.equ ONESEC,				5333333

.equ BSRR_12_SET,			(1 << 12)
.equ BSRR_12_RESET,			(1 << 28)

			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.global		__main


__main:
			bl		gpio_init
			bl		systick_init
loop:
			bl		blink
			b		loop


gpio_init:
			ldr r0,=RCC_AHB1ENR
			ldr r1,[r0]
			orr r1,#GPIOD_EN
			str r1,[r0]

			/*Set PD12 as output*/
			ldr r0,=GPIOD_MODER
			ldr r1,[r0]
			orr r1,#MODER_12_OUT
			str r1,[r0]

			mov r1,#0
			ldr r2,=GPIOD_BSRR
			bx 	lr

systick_init:
			/*Disable systick timer befor configuration*/
			ldr		r1,=SYST_CSR
			mov		r0,#0
			str		r0,[r1]

			/*Load maximum value into systick*/
			ldr		r1,=SYST_RVR
			ldr		r0,=SYSTICK_24BIT_MAX
			str		r0,[r1]

			/*Clear SYSTICK CURRENT VALUE refister
			by writing any value into it*/
			ldr		r1,=SYST_CVR
			mov		r0,#0
			str		r0,[r1]

			/*Select internal clock source and Enable Systick*/
			ldr		r0,=SYST_CSR
			ldr		r1,[r0]
			orr		r1,#SYST_CSR_CLKSRC
			orr		r1,#SYST_CSR_EN
			str		r1,[r0]
			bx		lr

/*Takes number of clockcycles to delay as argument*/
systick_delay:
			ldr		r1,=SYST_RVR
			sub		r0,#1
			str		r0,[r1]

lp1:

			/*Check if timeout has ockured*/
			ldr		r1,=SYST_CSR
			ldr		r3,[r1]
			ands	r3,r3,#SYST_CSR_CNTFLG
			beq		lp1
			bx		lr
blink:
			mov		r1,#LED_ON
			str		r1,[r2]

			ldr		r0,=900000
			bl		systick_delay

			mov		r1,#LED_OFF
			str		r1,[r2]

			ldr		r0,=900000
			bl		systick_delay

			b		blink


			.align
			.end
