

.equ RCC_BASE, 				0x40023800
.equ AHB1ENR_OFFSET,		0x30
.equ RCC_AHB1ENR,	 		(RCC_BASE + AHB1ENR_OFFSET)

.equ APB2ENR_OFFSET,		0x44
.equ RCC_APB2ENR,			(RCC_BASE + APB2ENR_OFFSET)

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

.equ AFRH_OFFSET,			0x24
.equ GPIOA_AFRH,			(GPIOA_BASE + AFRH_OFFSET)

.equ UART1_BASE,			0x40011000
.equ UART_BRR_OFFSET,		0x08
.equ UART1_BRR,				(UART1_BASE + UART_BRR_OFFSET)

.equ UART_CR1_OFFSET,		0x0C
.equ UART_CR2_OFFSET,		0x10
.equ UART_CR3_OFFSET,		0x14
.equ UART1_CR1,				(UART1_BASE + UART_CR1_OFFSET)
.equ UART1_CR2,				(UART1_BASE + UART_CR2_OFFSET)
.equ UART1_CR3,				(UART1_BASE + UART_CR3_OFFSET)
.equ UART_SR_OFFSET,		0x00
.equ UART1_SR,				(UART1_BASE + UART_SR_OFFSET)
.equ UART_DR_OFFSET,		0x04
.equ UART1_DR,				(UART1_BASE + UART_DR_OFFSET)

.equ GPIOD_EN,				(1 << 3)
.equ MODER_12_OUT,			(1 << 24)
.equ LED_ON,				(1 << 12)
.equ LED_OFF,				(1 << 0)
.equ ONESEC,				5333333
.equ GPIOA_EN,				(1 << 0)
.equ UART1_EN,				(1 << 4)


.equ BRR_CNF,				0x683 //9600 baudrate @16MHz
.equ CR1_CNF,				(0x0008) //enable Tx in UART, 8-bit data
.equ CR2_CNF,				(0x0000) //1 stop bit
.equ CR3_CNF,				(0x0000) //no flow control
.equ CR1_UARTEN,			(1 << 13) //enable UART

.equ SR_TXE,				(1 << 7)

.equ MODER9_ALT_SLT,		(1 << 19)
.equ AF7_SLT,				0x70

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
			bl		uart_init

loop:
			/*
			Test a single character
			'Y' = 0x59
			*/
			//mov		r0,#0x59
			mov		r0,#'A'
			bl		uart_outchar
			b		loop


uart_init:
			/*1. Enable clock access to UART GPIO pins*/
			ldr		r0,=RCC_AHB1ENR
			ldr		r1,[r0]
			orr		r1,#GPIOA_EN
			str		r1,[r0]

			/*2. Set UART gpio pin mode to alternate function mode*/
			ldr		r0,=GPIOA_MODER
			ldr		r1,[r0]
			bic		r1,#0xC0 //clear buits for PA9
			str		r1,[r0]

			ldr		r0,=GPIOA_MODER
			ldr		r1,[r0]
			orr		r1,#MODER9_ALT_SLT
			str		r1,[r0]

			/*3. Set UART gpio pin alternate function to UART*/
			ldr		r0,=GPIOA_AFRH
			ldr		r1,[r0]
			bic		r1,#0xF0
			str   	r1,[r0]

			ldr		r0,=GPIOA_AFRH
			ldr		r1,[r0]
			orr		r1,#AF7_SLT
			str		r1,[r0]

			/*4. Enable clock access to UART1 module*/
			ldr		r0,=RCC_APB2ENR
			ldr		r1,[r0]
			orr		r1,#UART1_EN
			str		r1,[r0]

			/*5. Set UART baudrate*/
			ldr		r0,=UART1_BRR
			mov		r1,#BRR_CNF
			str		r1,[r0]

			/*6. Configure control register 1*/
			ldr		r0,=UART1_CR1
			mov		r1,#CR1_CNF
			str		r1,[r0]

			/*7. Configure control register 2*/
			ldr		r0,=UART1_CR2
			mov		r1,#CR2_CNF
			str		r1,[r0]

			/*8. Configure control register 3*/
			ldr		r0,=UART1_CR3
			mov		r1,#CR3_CNF
			str		r1,[r0]

			/*9. Enable UART module*/
			ldr		r0,=UART1_CR1
			ldr		r1,[r0]
			orr		r1,#CR1_UARTEN
			str		r1,[r0]

			bx		lr

uart_outchar:
			/*1. Make sure UART transmit fifo is not full*/
			ldr		r1,=UART1_SR

lp2:		ldr		r2,[r1]
			and		r2,#SR_TXE
			cmp		r2,#0x00
			beq		lp2

			/*2. Write data into uart data register*/
			mov		r1,r0	/*Character to be send is placed in r0*/
			ldr		r2,=UART1_DR
			str		r1,[r2]
			bx		lr


stop:
			b	stop

			.align
			.end
