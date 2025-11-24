.data
print_fmt: .string "%ld \n"
.text
.globl doble
doble:
 pushq %rbp
 movq %rsp, %rbp
 subq $8, %rsp
 movq %rdi,-8(%rbp)
 movq -8(%rbp), %rax
 pushq %rax
 movq $2, %rax
 movq %rax, %rcx
 popq %rax
 imulq %rcx, %rax
 jmp .end_doble
.end_doble:
leave
ret
.globl triple
triple:
 pushq %rbp
 movq %rsp, %rbp
 subq $8, %rsp
 movq %rdi,-8(%rbp)
 movq -8(%rbp), %rax
 mov %rax, %rdi
call doble
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 jmp .end_triple
.end_triple:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $3, %rax
 mov %rax, %rdi
call triple
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $0, %eax
 leave
 ret
.section .note.GNU-stack,"",@progbits
