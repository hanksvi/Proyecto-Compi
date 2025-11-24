.data
print_fmt: .string "%ld \n"
b: .quad 0
a: .quad 0
.text
 movq $1, %rax
 movq %rax, a(%rip)
 movq $0, %rax
 movq %rax, b(%rip)
 movq a(%rip), %rax
 cmpq $0, %rax
 je else_0
 movq b(%rip), %rax
 cmpq $0, %rax
 je else_1
 movq $100, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp endif_1
 else_1:
endif_1:
 movq $200, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp endif_0
 else_0:
endif_0:
 movq $300, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.section .note.GNU-stack,"",@progbits
