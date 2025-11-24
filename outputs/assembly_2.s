.data
print_fmt: .string "%ld \n"
x: .quad 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $3, %rax
 pushq %rax
 movq $4, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 pushq %rax
 movq $2, %rax
 movq %rax, %rcx
 popq %rax
 imulq %rcx, %rax
 pushq %rax
 movq $10, %rax
 pushq %rax
 movq $5, %rax
 movq %rax, %rcx
 popq %rax
 cqto
 idivq %rcx
 movq %rax, %rcx
 popq %rax
 subq %rcx, %rax
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
