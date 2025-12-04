	.text
	.globl main
	.type main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$4, %rsp
	movl	$7, %eax
	movl	%eax, -4(%rbp)
	movl	-4(%rbp), %eax
	leave
	ret
