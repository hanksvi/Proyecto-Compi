.data
print_fmt: .string "%ld \n"
res: .quad 0
.text
.globl saludo
saludo:
 pushq %rbp
 movq %rsp, %rbp
 subq $0, %rsp
 movq $1, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $2, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $0, %rax
 jmp .end_saludo
.end_saludo:
leave
ret
.globl procesar
procesar:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi,-8(%rbp)
 subq $16, %rsp
 movq -8(%rbp), %rax
 pushq %rax
 movq $10, %rax
 movq %rax, %rcx
 popq %rax
 imulq %rcx, %rax
 movq %rax, -16(%rbp)
 movq -16(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -16(%rbp), %rax
 jmp .end_procesar
.end_procesar:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 subq $8, %rsp
 movq $3, %rax
 mov %rax, %rdi
call procesar
 movq %rax, -8(%rbp)
 movq -8(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $0, %rax
 jmp .end_main
.end_main:
leave
ret
.section .note.GNU-stack,"",@progbits
