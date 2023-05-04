	.file "func.c"
	.text
	.globl fib
	.type fib, @function
fib:
LFB0:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl %edi, -4(%rbp)
	movl %esi, -8(%rbp)
	cmpl $1000, -4(%rbp)
	jg .L0
	jmp .L1
.L0:
	movl -4(%rbp), %eax
	imull -8(%rbp), %eax
	cltd
	movl $5, %ecx
	idivl %ecx
	movl %edx, %eax
	subl $-1, %eax
	jmp .L2
.L1:
	movl -4(%rbp), %edx
	movl -8(%rbp), %eax
	addl %eax, %edx
	movl -4(%rbp), %esi
	movl %edx, %edi
	CALL fib
	movl %eax, %edx
	movl $1, %eax
	addl %eax, %edx
	movl %edx, %eax
	jmp .L2
.L2:
	leave
	ret
.LC0:
	.string "%d\n"
	.text
	.globl main
	.type main, @function
main:
LFB1:
	pushq %rbp
	movq %rsp, %rbp
	movl $1, %esi
	movl $1, %edi
	CALL fib
	movl %eax, %esi
	leaq .LC0(%rip), %rax
	movq %rax, %rdi
	movl $0, %eax
	CALL printf@PLT
	movl $0, %eax
	jmp .L3
.L3:
	popq %rbp
	ret
