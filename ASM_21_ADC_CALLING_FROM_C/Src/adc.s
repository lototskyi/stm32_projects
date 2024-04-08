

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

.equ ADC1_BASE,				0x40012000
.equ ADC_CR2_OFFSET,		0x08
.equ ADC1_CR2,				(ADC1_BASE + ADC_CR2_OFFSET)

.equ ADC_SQR1_OFFSET,		0x2C
.equ ADC1_SQR1,				(ADC1_BASE + ADC_SQR1_OFFSET)

.equ ADC_SQR3_OFFSET,		0x34
.equ ADC1_SQR3,				(ADC1_BASE + ADC_SQR3_OFFSET)

.equ ADC_SR_OFFSET,			0x00
.equ ADC1_SR,				(ADC1_BASE + ADC_SR_OFFSET)

.equ ADC_DR_OFFSET,			0x4C
.equ ADC1_DR,				(ADC1_BASE + ADC_DR_OFFSET)

.equ IDR_OFFSET,			0x10
.equ GPIOA_IDR,				(GPIOA_BASE + IDR_OFFSET)

.equ GPIOD_EN,				(1 << 3)
.equ MODER_12_OUT,			(1 << 24)
.equ LED_ON,				(1 << 12)
.equ LED_OFF,				(1 << 0)
.equ ONESEC,				5333333
.equ GPIOA_EN,				(1 << 0)

.equ ADC1_EN,				(1<<8)
.equ CR2_ADC_ON,			(1<<0)
.equ SQR3_CNF,				1 /*Conversion sequence starts at ch1*/
.equ SQR1_CNF,				0 /*Conversion sequence length to 1*/
.equ CR2_SWSTART,			(1<<30)
.equ SR_EOC,				(1<<1)

.equ SENS_THRESH,			3000

.equ BSRR_12_SET,			(1 << 12)
.equ BSRR_12_RESET,			(1 << 28)

.equ MODER1_ANLG_SLT,		0xC


			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.global		adc_init
			.global		adc_read
			.global		led_init
			.global		led_control
			.global		led_on
			.global		led_off


adc_init:

			/*1. Enable clock access to ADC pin's GPIO port*/
			ldr r0,=RCC_AHB1ENR
			ldr r1,[r0]
			orr r1,#GPIOA_EN
			str r1,[r0]

			/*2. Set ADC pin, PA1 as analog pin*/
			ldr	r0,=GPIOA_MODER
			ldr	r1,[r0]
			orr	r1,#MODER1_ANLG_SLT
			str r1,[r0]

			/*3. Enable clock access to the ADC*/
			ldr r0,=RCC_APB2ENR
			ldr r1,[r0]
			orr r1,#ADC1_EN
			str r1,[r0]

			/*4. Select software trigger*/
			ldr r0,=ADC1_CR2
			ldr r1,=0x00000000
			str r1,[r0]

			/*5. Set conversion sequence starting channel*/
			ldr r0,=ADC1_SQR3
			ldr r1,=SQR3_CNF
			str r1,[r0]

			/*6. Set conversion sequence length*/
			ldr r0,=ADC1_SQR1
			ldr r1,=SQR1_CNF
			str r1,[r0]

			/*7. Enable ADC module*/
			ldr r0,=ADC1_CR2
			ldr r1,[r0]
			orr r1,#CR2_ADC_ON
			str r1,[r0]

			bx	lr

adc_read:

			/*1. Start conversion*/
			ldr r0,=ADC1_CR2
			ldr r1,[r0]
			orr r1,#CR2_SWSTART
			str r1,[r0]

			/*2. wait for conversion to be complete*/
lp1:		ldr r0,=ADC1_SR
			ldr r1,[r0]
			and r1,#SR_EOC
			cmp r1,#0x00
			beq lp1

			/*3. Read content of ADC data register*/
			ldr r2,=ADC1_DR
			ldr r0,[r2]
			bx	lr

led_init:
			/*Set PD12 as output*/
			ldr r0,=RCC_AHB1ENR
			ldr r1,[r0]
			orr r1,#GPIOD_EN
			str r1,[r0]

			ldr r0,=GPIOD_MODER
			ldr r1,[r0]
			orr r1,#MODER_12_OUT
			str r1,[r0]
			bx	lr

led_control:
			ldr r1,=SENS_THRESH
			cmp	r0,r1
			bgt	led_on  //Branch to led_on if ADC value is greater than SENS_THRESH
			blt led_off //Branch to led_off if ADC value is less than SENS_THRESH
			bx	lr

led_on:
			ldr r5,=GPIOD_BSRR
			mov r1,#BSRR_12_SET
			str	r1,[r5]
			bx	lr

led_off:
			ldr r5,=GPIOD_BSRR
			mov r1,#BSRR_12_RESET
			str	r1,[r5]
			bx	lr

stop:
			b	stop

			.align
			.end
