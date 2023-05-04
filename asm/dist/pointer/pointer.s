	.file "pointer.c"
.LC0:
	.string "%d\n"
	.text
	.globl main
	.type main, @function
main:
LFB0:
	pushq %rbp
	movq %rsp, %rbp
	subq $32, %rsp
	movl $2, -16(%rbp)
	movl $3, -20(%rbp)
	movl -16(%rbp), %esi
	leaq .LC0(%rip), %rax
	movq %rax, %rdi
	movl $0, %eax
	CALL printf@PLT
	movl $0, %eax
	movl -20(%rbp), %esi
	leaq .LC0(%rip), %rax
	movq %rax, %rdi
	movl $0, %eax
	CALL printf@PLT
	movl $0, %eax
	leaq -16(%rbp), %rax
	movq %rax, -4(%rbp)
	leaq -20(%rbp), %rax
	movq %rax, -12(%rbp)
	movq -12(%rbp), %rax
	movl (%rax), %ecx
	movl %ecx, -24(%rbp)
	movq -4(%rbp), %rax
	movl (%rax), %ecx
	movq -12(%rbp), %rax
	movl %ecx, (%rax)
	movq -12(%rbp), %rax
	movl (%rax), %ecx
	movl -24(%rbp), %eax
	imull %ecx, %eax
	movq -4(%rbp), %rbx
	movl (%rbx), %ecx
	imull %ecx, %eax
	movq -12(%rbp), %rbx
	movl (%rbx), %ecx
	cltd
	idivl %ecx
	movq -4(%rbp), %rdx
	movl %eax, (%rdx)
	movq -4(%rbp), %rax
	movl (%rax), %ecx
	movl %ecx, %esi
	leaq .LC0(%rip), %rax
	movq %rax, %rdi
	movl $0, %eax
	CALL printf@PLT
	movl $0, %eax
	movq -12(%rbp), %rax
	movl (%rax), %ecx
	movl %ecx, %esi
	leaq .LC0(%rip), %rax
	movq %rax, %rdi
	movl $0, %eax
	CALL printf@PLT
	movl $0, %eax
	jmp .L0
.L0:
	leave
	ret
