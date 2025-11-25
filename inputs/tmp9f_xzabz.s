.data
print_fmt: .string "%ld \n"
.text
.globl suma
suma:
 pushq %rbp
 movq %rsp, %rbp
 subq $24, %rsp
 movq %rdi,-8(%rbp)
 movq %rsi,-16(%rbp)
 movq -8(%rbp), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, c(%rip)
 movq c(%rip), %rax
 jmp .end_suma
.end_suma:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $5, %rax
 mov %rax, %rdi
 movq $10, %rax
 mov %rax, %rsi
call suma
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $0, %eax
 leave
 ret
.section .note.GNU-stack,"",@progbits
