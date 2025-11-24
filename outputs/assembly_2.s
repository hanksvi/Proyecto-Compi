.data
print_fmt: .string "%ld \n"
y: .quad 0
x: .quad 0
.text
 movq $5, %rax
 movq %rax, x(%rip)
 movq $3.14, %rax
 movq %rax, y(%rip)
 movq y(%rip), %rax
 movq %rax, x(%rip)
.section .note.GNU-stack,"",@progbits
