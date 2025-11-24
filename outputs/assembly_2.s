.data
print_fmt: .string "%ld \n"
r: .quad 0
b: .quad 0
a: .quad 0
.text
.globl mayor
mayor:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi,-8(%rbp)
 movq %rsi,-16(%rbp)
 subq $16, %rsp
 movq -8(%rbp), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 cmpq $0, %rax
 je else_0
 movq -8(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -8(%rbp), %rax
 jmp .end_mayor
 jmp endif_0
 else_0:
 movq -16(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -16(%rbp), %rax
 jmp .end_mayor
endif_0:
.end_mayor:
leave
ret
 movq $15, %rax
 movq %rax, a(%rip)
 movq $9, %rax
 movq %rax, b(%rip)
 movq a(%rip), %rax
 mov %rax, %rdi
 movq b(%rip), %rax
 mov %rax, %rsi
call mayor
 movq %rax, r(%rip)
.section .note.GNU-stack,"",@progbits
