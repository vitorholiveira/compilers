	.text
	.globl main
	.type main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$60, %rsp
	movl	$10, %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	$0, %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	$1, %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	movl	%eax, -12(%rbp)
L0:
	movl	-12(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -32(%rbp)
	movl	-28(%rbp), %eax
	movl	-32(%rbp), %ecx
	cmpl	%ecx, %eax
	setle	%al
	movzbl	%al, %eax
	movl	%eax, -36(%rbp)
	movl	-36(%rbp), %eax
	cmpl	$0, %eax
	jne	L1
	jmp	L2
L1:
	movl	-8(%rbp), %eax
	movl	%eax, -40(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -44(%rbp)
	movl	-40(%rbp), %eax
	movl	-44(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -48(%rbp)
	movl	-48(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -52(%rbp)
	movl	$1, %eax
	movl	%eax, -56(%rbp)
	movl	-52(%rbp), %eax
	movl	-56(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -60(%rbp)
	movl	-60(%rbp), %eax
	movl	%eax, -12(%rbp)
	jmp	L0
L2:
	movl	-4(%rbp), %eax
	movl	$0, %eax
	leave
	ret
