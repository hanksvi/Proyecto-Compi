.data
print_fmt: .string "%ld \n"
y: .quad 0
x: .quad 0
.text
 movq $10, %rax
 movq %rax, x(%rip)
 movq x(%rip), %rax
 pushq %rax
 movq $5, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, y(%rip)
 movq y(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.section .note.GNU-stack,"",@progbits
