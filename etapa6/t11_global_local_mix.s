	.data
	.align 4
rbss_data:
	.zero 4

	.text
	.globl main
	.type main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	leaq	rbss_data(%rip), %rbx
	subq	$24, %rsp
	movl	$200, %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, 0(%rbx)
	movl	$100, %eax
	movl	%eax, -12(%rbp)
	movl	-12(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	0(%rbx), %eax
	movl	%eax, -16(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -20(%rbp)
	movl	-16(%rbp), %eax
	movl	-20(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	leave
	ret
