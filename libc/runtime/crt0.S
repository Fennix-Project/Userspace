# https://wiki.osdev.org/Creating_a_C_Library
.section .text

.global _start
_start:
	movq $0, %rbp
	pushq %rbp
	pushq %rbp
	movq %rsp, %rbp

	pushq %rsi
	pushq %rdi

	call __libc_init
	call _init

	popq %rdi
	popq %rsi

	call main
	movl %eax, %edi
	call _exit
.size _start, . - _start
