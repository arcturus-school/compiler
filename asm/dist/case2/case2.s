	.file "case2.c"
.LC0:
	.string "%d %d\n"
	.text
	.globl main
	.type main, @function
main:
LFB0:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $1, -4(%rbp)
	movl $1, -8(%rbp)
	movl $2, -12(%rbp)
.L2:
	cmpl $20, -12(%rbp)
	jl .L0
	jmp .L1
.L0:
	movl -4(%rbp), %eax
	movl %eax, -16(%rbp)
	movl -4(%rbp), %edx
	movl -8(%rbp), %eax
	addl %eax, %edx
	movl %edx, -4(%rbp)
	movl -16(%rbp), %eax
	movl %eax, -8(%rbp)
	movl -12(%rbp), %edx
	movl $1, %eax
	addl %eax, %edx
	movl %edx, -12(%rbp)
	jmp .L2
.L1:
	movl -8(%rbp), %edx
	movl -4(%rbp), %esi
	leaq .LC0(%rip), %rax
	movq %rax, %rdi
	movl $0, %eax
	CALL printf@PLT
	movl $0, %eax
	jmp .L3
.L3:
	leave
	ret
