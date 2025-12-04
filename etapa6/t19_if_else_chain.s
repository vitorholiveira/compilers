	.text
	.globl main
	.type main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$48, %rsp
	movl	$0, %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	$1, %eax
	movl	%eax, -12(%rbp)
	movl	$2, %eax
	movl	%eax, -16(%rbp)
	movl	-12(%rbp), %eax
	movl	-16(%rbp), %ecx
	cmpl	%ecx, %eax
	setg	%al
	movzbl	%al, %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	cmpl	$0, %eax
	jne	L0
	jmp	L1
L0:
	movl	$1, %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	movl	%eax, -4(%rbp)
	jmp	L2
L1:
	movl	$2, %eax
	movl	%eax, -28(%rbp)
	movl	$3, %eax
	movl	%eax, -32(%rbp)
	movl	-28(%rbp), %eax
	movl	-32(%rbp), %ecx
	cmpl	%ecx, %eax
	setg	%al
	movzbl	%al, %eax
	movl	%eax, -36(%rbp)
	movl	-36(%rbp), %eax
	cmpl	$0, %eax
	jne	L3
	jmp	L4
L3:
	movl	$2, %eax
	movl	%eax, -40(%rbp)
	movl	-40(%rbp), %eax
	movl	%eax, -4(%rbp)
	jmp	L5
L4:
	movl	$3, %eax
	movl	%eax, -44(%rbp)
	movl	-44(%rbp), %eax
	movl	%eax, -4(%rbp)
L5:
L2:
	movl	-4(%rbp), %eax
	movl	%eax, -48(%rbp)
	movl	-48(%rbp), %eax
	leave
	ret
