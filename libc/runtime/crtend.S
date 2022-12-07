# I WILL PLACE HERE THE crtn.o BECAUSE GCC REFUSE TO LINK IT AND I DON'T KNOW WHY
.section .init
	popq %rbp
	ret

.section .fini
	popq %rbp
	ret
