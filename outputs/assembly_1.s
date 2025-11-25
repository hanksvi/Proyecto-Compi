.data
print_fmt: .string "%ld\n"
print_fmt_uint: .string "%lu\n"
print_fmt_float: .string "%f\n"
print_fmt_true: .string "true\n"
print_fmt_false: .string "false\n"
.text
.globl calcularPromedio
calcularPromedio:
 pushq %rbp
 movq %rsp, %rbp
 subq $32, %rsp
 movq %rdi,-8(%rbp)
 movq %rsi,-16(%rbp)
 movq %rdx,-24(%rbp)
 movq -8(%rbp), %rax
 cvtsi2sd %rax, %xmm0
 movq %xmm0, %rax
movq %rax, %xmm0
pushq %rax
 movq -16(%rbp), %rax
 cvtsi2sd %rax, %xmm0
 movq %xmm0, %rax
movq %rax, %xmm1
pop %rax
movq %rax, %xmm0
 addsd %xmm1, %xmm0
 movq %xmm0, %rax
movq %rax, %xmm0
pushq %rax
 movq -24(%rbp), %rax
movq %rax, %xmm1
pop %rax
movq %rax, %xmm0
 addsd %xmm1, %xmm0
 movq %xmm0, %rax
 movq %rax, -32(%rbp)
 movq -32(%rbp), %rax
movq %rax, %xmm0
pushq %rax
 movq $3, %rax
 cvtsi2sd %rax, %xmm0
 movq %xmm0, %rax
movq %rax, %xmm1
pop %rax
movq %rax, %xmm0
 divsd %xmm1, %xmm0
 movq %xmm0, %rax
 jmp .end_calcularPromedio
.end_calcularPromedio:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $10, %rax
 mov %rax, %rdi
 movq $20, %rax
 mov %rax, %rsi
 movsd float_const_0(%rip), %xmm0
 movq %xmm0, %rax
 mov %rax, %rdx
call calcularPromedio
 movq %rax, -40(%rbp)
 movq -40(%rbp), %rax
 movq %rax, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 movl $0, %eax
 leave
 ret
.section .rodata
.align 8
float_const_0:
 .double 15.5
.section .note.GNU-stack,"",@progbits
