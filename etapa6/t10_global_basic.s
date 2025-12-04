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
	subq	$8, %rsp
	movl	$200, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, 0(%rbx)
	movl	0(%rbx), %eax
	movl	%eax, -8(%rbp)
	movl	-8(%rbp), %eax
	leave
	ret
