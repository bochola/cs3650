	.file	"cfib.c"
	.text
	.globl	cfib
	.type	cfib, @function
cfib:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%rbx
	subq	$40, %rsp
	.cfi_offset 3, -24
	movl	%edi, -36(%rbp)
	cmpl	$1, -36(%rbp)
	jle	.L2
	movl	-36(%rbp), %eax
	subl	$1, %eax
	movl	%eax, %edi
	call	cfib
	movl	%eax, %ebx
	movl	-36(%rbp), %eax
	subl	$2, %eax
	movl	%eax, %edi
	call	cfib
	addl	%ebx, %eax
	movl	%eax, -20(%rbp)
	jmp	.L3
.L2:
	cmpl	$1, -36(%rbp)
	jne	.L4
	movl	-36(%rbp), %eax
	movl	%eax, -20(%rbp)
	jmp	.L3
.L4:
	movl	$0, -20(%rbp)
.L3:
	movl	-20(%rbp), %eax
	addq	$40, %rsp
	popq	%rbx
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	cfib, .-cfib
	.ident	"GCC: (Debian 8.3.0-6) 8.3.0"
	.section	.note.GNU-stack,"",@progbits
