.data
print_fmt: .string "%ld \n"
y: .quad 0
x: .quad 0
.text
 movq %rax, x(%rip)
.section .note.GNU-stack,"",@progbits
