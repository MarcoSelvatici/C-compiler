	.file	1 "in.c"
	.section .mdebug.abi32
	.previous
	.nan	legacy
	.module	fp=32
	.module	oddspreg
	.abicalls
	.text

	.comm	a,4,4
	.globl	b
	.data
	.align	2
	.type	b, @object
	.size	b, 4
b:
	.word	3
	.text
	.align	2
	.globl	main
	.set	nomips16
	.set	nomicromips
	.ent	main
	.type	main, @function
main:
	.frame	$fp,8,$31		# vars= 0, regs= 1/0, args= 0, gp= 0
	.mask	0x40000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.set	nomacro
	addiu	$sp,$sp,-8
	sw	$fp,4($sp)
	move	$fp,$sp
	lui	$28,%hi(__gnu_local_gp)
	addiu	$28,$28,%lo(__gnu_local_gp)
	lw	$2,%got(a)($28)
	li	$3,2			# 0x2
	sw	$3,0($2)
	lw	$2,%got(a)($28)
	lw	$3,0($2)
	lui	$2,%hi(b)
	lw	$2,%lo(b)($2)
	addu	$2,$3,$2
	move	$sp,$fp
	lw	$fp,4($sp)
	addiu	$sp,$sp,8
	jr	$31
	nop

	.set	macro
	.set	reorder
	.end	main
	.size	main, .-main
	.ident	"GCC: (Ubuntu 7.3.0-27ubuntu1~18.04) 7.3.0"
