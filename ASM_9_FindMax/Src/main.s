

count	.req	r0
max		.req	r1
pointer	.req	r2
next	.req	r3

mydata:		.word	69,87,86,45,75


			.syntax unified
			.cpu	cortex-m4
			.fpu	softvfp
			.thumb
			.section	.text
			.global		__main


__main:
			mov count,#5
			mov	 max,#0

			//load mem address of mydata into r2
			ldr	 pointer,=mydata
again:
			//load value at mem address found in r2 into r3
			ldr	 next,[pointer]

			//compare max width r3 and update conditional flags
			cmp	 max,next

			//branch to ctnu if max is equal or greater than r3
			bhs ctnu

			mov max,next

ctnu:
			//add 4 bytes to r3 and store result in r3
			add pointer,pointer,#4

			//subtract 1 from count, store result in count
			//update conditional flag
			subs count,count,#1

			//branch if result is not equal to zero
			bne again


stop:
			b	stop


			.align
			.end
