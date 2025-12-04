	.text
	.globl main
	.type main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$44, %rsp
	movl	$5, %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	$1, %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, -8(%rbp)
L0:
	movl	-4(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -24(%rbp)
	movl	-20(%rbp), %eax
	movl	-24(%rbp), %ecx
	cmpl	%ecx, %eax
	setl	%al
	movzbl	%al, %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	cmpl	$0, %eax
	jne	L1
	jmp	L2
L1:
	movl	-4(%rbp), %eax
	movl	%eax, -32(%rbp)
	movl	$1, %eax
	movl	%eax, -36(%rbp)
	movl	-32(%rbp), %eax
	movl	-36(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -40(%rbp)
	movl	-40(%rbp), %eax
	movl	%eax, -4(%rbp)
	jmp	L0
L2:
	movl	$0, %eax
	movl	%eax, -44(%rbp)
	movl	-44(%rbp), %eax
	leave
	ret
