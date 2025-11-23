.data
print_fmt: .string "%ld \n"
c: .quad 0
b: .quad 0
a: .quad 0
.text
.globl cuadrado
cuadrado:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi,-8(%rbp)
 subq $8, %rsp
 movq -8(%rbp), %rax
 pushq %rax
 movq -8(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 imulq %rcx, %rax
 jmp .end_cuadrado
.end_cuadrado:
leave
ret
.globl suma
suma:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi,-8(%rbp)
 movq %rsi,-16(%rbp)
 subq $16, %rsp
 movq a(%rip), %rax
 pushq %rax
 movq b(%rip), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 jmp .end_suma
.end_suma:
leave
ret
.globl operacion
operacion:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi,-8(%rbp)
 movq %rsi,-16(%rbp)
 subq $24, %rsp
 movq -8(%rbp), %rax
 mov %rax, %rdi
call cuadrado
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 pushq %rax
 movq -8(%rbp), %rax
 mov %rax, %rdi
 movq -16(%rbp), %rax
 mov %rax, %rsi
call suma
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 jmp .end_operacion
.end_operacion:
leave
ret
.section .note.GNU-stack,"",@progbits
