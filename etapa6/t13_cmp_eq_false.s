	.text
	.globl main
	.type main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$20, %rsp
	movl	$3, %eax
	movl	%eax, -8(%rbp)
	movl	$4, %eax
	movl	%eax, -12(%rbp)
	movl	-8(%rbp), %eax
	movl	-12(%rbp), %ecx
	cmpl	%ecx, %eax
	sete	%al
	movzbl	%al, %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	leave
	ret
