.data
print_fmt: .string "%ld \n"
x: .quad 0
.text
 movq $100, %rax
 movq %rax, x(%rip)
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.section .note.GNU-stack,"",@progbits
