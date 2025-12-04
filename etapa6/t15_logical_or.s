	.text
	.globl main
	.type main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$36, %rsp
	movl	$1, %eax
	movl	%eax, -8(%rbp)
	movl	$2, %eax
	movl	%eax, -12(%rbp)
	movl	-8(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	setg	%al
	movzbl	%al, %eax
	movl	%eax, -16(%rbp)
	movl	$3, %eax
	movl	%eax, -20(%rbp)
	movl	$3, %eax
	movl	%eax, -24(%rbp)
	movl	-20(%rbp), %eax
	movl	-24(%rbp), %ecx
	cmpl	%ecx, %eax
	sete	%al
	movzbl	%al, %eax
	movl	%eax, -28(%rbp)
	movl	-16(%rbp), %eax
	movl	-28(%rbp), %ecx
	orl	%ecx, %eax
	movl	%eax, -32(%rbp)
	movl	-32(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -36(%rbp)
	movl	-36(%rbp), %eax
	leave
	ret
