	.text
	.globl main
	.type main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$16, %rsp
	movl	$0, %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	movl	$1, %ecx
	xorl	%ecx, %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	leave
	ret
