# https://wiki.osdev.org/Creating_a_C_Library
.section .text

.global _start
_start:
	movq $0, %rbp
	pushq %rbp
	pushq %rbp
	movq %rsp, %rbp

	pushq %rcx
	pushq %rdx
	pushq %rsi
	pushq %rdi

	call __libc_init

	popq %rdi
	popq %rsi
	popq %rdx
	popq %rcx

	call main
	movl %eax, %edi
	call _exit
.size _start, . - _start
