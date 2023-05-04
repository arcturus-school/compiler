	.file "case1.c"
.LC0:
	.string "%d\n"
	.text
	.globl main
	.type main, @function
main:
LFB0:
	pushq %rbp
	movq %rsp, %rbp
	subq $16, %rsp
	movl $50, -4(%rbp)
	movl $-1, -8(%rbp)
	movl $0, -12(%rbp)
.L15:
	movl -4(%rbp), %eax
	imull $2, %eax
	cmpl -12(%rbp), %eax
	jg .L0
	jmp .L1
.L0:
	movl $0, -16(%rbp)
.L14:
	movl -16(%rbp), %eax
	cmpl %eax, -4(%rbp)
	jg .L2
	jmp .L3
.L2:
	movl -4(%rbp), %eax
	cltd
	movl $2, %ecx
	idivl %ecx
	cmpl -16(%rbp), %eax
	setg %al
	movzbl %al, %eax
	cmpl $0, %eax
	setne %al
	movzbl %al, %eax
	jne .L5
	jmp .L6
.L5:
	movl -16(%rbp), %eax
	cltd
	movl $2, %ecx
	idivl %ecx
	cmpl $0, %edx
	setne %al
	movzbl %al, %eax
	cmpl $0, %eax
	setne %al
	movzbl %al, %eax
	jne .L8
	jmp .L6
.L8:
	movl -12(%rbp), %eax
	imull -16(%rbp), %eax
	cltd
	movl $2, %ecx
	idivl %ecx
	movl -8(%rbp), %edx
	addl %eax, %edx
	movl -12(%rbp), %eax
	imull -16(%rbp), %eax
	movl %edx, %edx
	addl %eax, %edx
	movl %edx, -8(%rbp)
	jmp .L9
.L6:
	movl -8(%rbp), %eax
	cmpl -16(%rbp), %eax
	setg %al
	movzbl %al, %eax
	cmpl $0, %eax
	sete %al
	movzbl %al, %eax
	je .L11
	jmp .L12
.L11:
	cmpl $0, -8(%rbp)
	sete %al
	movzbl  %al, %eax
	cltd
	movl $2, %ecx
	idivl %ecx
	cmpl $1, %edx
	sete %al
	movzbl %al, %eax
	cmpl $0, %eax
	setne %al
	movzbl %al, %eax
	jne .L12
	jmp .L9
.L12:
	movl $3, %eax
	cltd
	movl $2, %ecx
	idivl %ecx
	movl -8(%rbp), %edx
	subl %eax, %edx
	movl %edx, -8(%rbp)
	jmp .L6
.L9:
	movl -16(%rbp), %edx
	movl $1, %eax
	addl %eax, %edx
	movl %edx, -16(%rbp)
	jmp .L14
.L3:
	movl -12(%rbp), %edx
	movl $1, %eax
	addl %eax, %edx
	movl %edx, -12(%rbp)
	jmp .L15
.L1:
	movl -8(%rbp), %esi
	leaq .LC0(%rip), %rax
	movq %rax, %rdi
	movl $0, %eax
	CALL printf@PLT
	movl $0, %eax
	jmp .L16
.L16:
	leave
	ret
