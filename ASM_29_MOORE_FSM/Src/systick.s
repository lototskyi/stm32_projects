.equ SYST_CSR,				0xE000E010
.equ SYST_RVR,				0xE000E014
.equ SYST_CVR,				0xE000E018

.equ SYSTICK_24BIT_MAX,		0x00FFFFFF
.equ SYST_CSR_EN,			(1 << 0)
.equ SYST_CSR_CLKSRC,		(1 << 2)
.equ SYST_CSR_CNTFLG,		(1 << 16)

/*
Default frequency = 16MHz
1 millisecond = 16000 cycles
*/
.equ DELAY1MS,				16000

			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.globl		systick_init
			.globl		systick_delay
			.globl		systick_delay_ms


systick_init:
			/*Disable systick timer befor configuration*/
			ldr		r1,=SYST_CSR
			mov		r0,#0
			str		r0,[r1]

			/*Load maximum value into systick*/
			ldr		r1,=SYST_RVR
			ldr		r0,=SYSTICK_24BIT_MAX
			str		r0,[r1]

			/*Clear SYSTICK CURRENT VALUE register
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


systick_delay_ms:
			push	{r4, lr} //save current value of r4 and lr
			movs	r4,r0
			beq		cmplt
lp2:
			ldr		r0,=DELAY1MS
			bl		systick_delay
			subs	r4,r4,#1
			bhi		lp2	/*if r4>0 delay another 1ms*/

cmplt:
			pop		{r4,lr} //restore previous values of r4 and lr
			bx		lr

			.align
			.end
