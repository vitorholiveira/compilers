	.text
	.globl main
	.type main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$24, %rsp
	movl	$5, %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	$10, %eax
	movl	%eax, -16(%rbp)
	movl	-12(%rbp), %eax
	movl	-16(%rbp), %ecx
	cmpl	%ecx, %eax
	setl	%al
	movzbl	%al, %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	cmpl	$0, %eax
	jne	L0
	jmp	L1
L0:
	movl	$20, %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	movl	%eax, -4(%rbp)
L1:
L2:
	movl	-4(%rbp), %eax
	leave
	ret
