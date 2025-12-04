	.text
	.globl main
	.type main, @function
main:
	pushq	%rbp
	movq	%rsp, %rbp
	subq	$116, %rsp
	movl	$10, %eax
	movl	%eax, -20(%rbp)
	movl	-20(%rbp), %eax
	movl	%eax, -4(%rbp)
	movl	$3, %eax
	movl	%eax, -24(%rbp)
	movl	-24(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -28(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -32(%rbp)
	movl	-28(%rbp), %eax
	movl	-32(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -36(%rbp)
	movl	-36(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -40(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -44(%rbp)
	movl	-40(%rbp), %eax
	movl	-44(%rbp), %ecx
	subl	%ecx, %eax
	movl	%eax, -48(%rbp)
	movl	-48(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -52(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -56(%rbp)
	movl	-52(%rbp), %eax
	movl	-56(%rbp), %ecx
	imull	%ecx, %eax
	movl	%eax, -60(%rbp)
	movl	-60(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -64(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -68(%rbp)
	movl	-64(%rbp), %eax
	movl	-68(%rbp), %ecx
	cltd
	idivl	%ecx
	movl	%eax, -72(%rbp)
	movl	-72(%rbp), %eax
	movl	%eax, -12(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -76(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -80(%rbp)
	movl	-76(%rbp), %eax
	movl	-80(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -84(%rbp)
	movl	-4(%rbp), %eax
	movl	%eax, -88(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -92(%rbp)
	movl	-88(%rbp), %eax
	movl	-92(%rbp), %ecx
	subl	%ecx, %eax
	movl	%eax, -96(%rbp)
	movl	-84(%rbp), %eax
	movl	-96(%rbp), %ecx
	imull	%ecx, %eax
	movl	%eax, -100(%rbp)
	movl	-8(%rbp), %eax
	movl	%eax, -104(%rbp)
	movl	$1, %eax
	movl	%eax, -108(%rbp)
	movl	-104(%rbp), %eax
	movl	-108(%rbp), %ecx
	addl	%ecx, %eax
	movl	%eax, -112(%rbp)
	movl	-100(%rbp), %eax
	movl	-112(%rbp), %ecx
	cltd
	idivl	%ecx
	movl	%eax, -116(%rbp)
	movl	-116(%rbp), %eax
	movl	%eax, -16(%rbp)
	movl	-4(%rbp), %eax
	leave
	ret
