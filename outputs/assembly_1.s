.data
print_fmt: .string "%ld \n"
.text
.globl doble
doble:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi,-8(%rbp)
 subq $8, %rsp
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
 movq $3.14, %rax
 movq %rax, y(%rip)
 movq $2.4, %rax
 mov %rax, %rdi
call doble
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.section .note.GNU-stack,"",@progbits
