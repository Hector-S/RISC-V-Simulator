	.file	"main.c"
	.option pic
	.attribute arch, "rv32i2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.align	2
	.globl	Third
	.type	Third, @function
Third:
	addi	sp,sp,-16
	sw	s0,12(sp)
	addi	s0,sp,16
	nop
	lw	s0,12(sp)
	addi	sp,sp,16
	jr	ra
	.size	Third, .-Third
	.align	2
	.globl	Second
	.type	Second, @function
Second:
	addi	sp,sp,-16
	sw	ra,12(sp)
	sw	s0,8(sp)
	addi	s0,sp,16
	call	Third@plt
	nop
	lw	ra,12(sp)
	lw	s0,8(sp)
	addi	sp,sp,16
	jr	ra
	.size	Second, .-Second
	.align	2
	.globl	First
	.type	First, @function
First:
	addi	sp,sp,-16
	sw	ra,12(sp)
	sw	s0,8(sp)
	addi	s0,sp,16
	call	Second@plt
	nop
	lw	ra,12(sp)
	lw	s0,8(sp)
	addi	sp,sp,16
	jr	ra
	.size	First, .-First
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-16
	li	ra,0
	sw	ra,12(sp)
	sw	s0,8(sp)
	addi	s0,sp,16
	call	First@plt
	li	a5,0
	mv	a0,a5
	lw	ra,12(sp)
	lw	s0,8(sp)
	addi	sp,sp,16
	jr	ra
	.size	main, .-main
	.ident	"GCC: (GNU) 11.1.0"
