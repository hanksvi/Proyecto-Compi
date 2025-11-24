.data
print_fmt: .string "%ld \n"
x: .quad 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $2, %rax
 movq %rax, x(%rip)
 movq x(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $0, %eax
 leave
 ret
.section .note.GNU-stack,"",@progbits
