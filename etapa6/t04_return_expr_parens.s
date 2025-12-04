	.text
	.globl main
	.type main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$28, %rsp
	movl	$1, %eax
	movl	%eax, -8(%rbp)
	movl	$2, %eax
	movl	%eax, -12(%rbp)
	movl	-8(%rbp), %eax
	movl	-12(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -16(%rbp)
	movl	$5, %eax
	movl	%eax, -20(%rbp)
	movl	-16(%rbp), %eax
	movl	-20(%rbp), %ecx
	subl	%ecx, %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	leave
	ret
