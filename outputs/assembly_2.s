.data
print_fmt: .string "%ld\n"
print_fmt_uint: .string "%lu\n"
print_fmt_float: .string "%f\n"
print_fmt_true: .string "true\n"
print_fmt_false: .string "false\n"
.text
.globl calcular
calcular:
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
 cmpq %rcx, %rax
 movl $0, %eax
 setg %al
 movzbq %al, %rax
 cmpq $0, %rax
 je ifexp_else_0
 movq -8(%rbp), %rax
 cvtsi2sdq %rax, %xmm0
 movq %xmm0, %rax
 jmp ifexp_end_0
ifexp_else_0:
 movq -16(%rbp), %rax
 cvtsi2sdq %rax, %xmm0
 movq %xmm0, %rax
ifexp_end_0:
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
movq %rax, %xmm0
pushq %rax
 movsd float_const_0(%rip), %xmm0
 movq %xmm0, %rax
movq %rax, %xmm1
pop %rax
movq %rax, %xmm0
 mulsd %xmm1, %xmm0
 movq %xmm0, %rax
 jmp .end_calcular
.end_calcular:
leave
ret
.globl seleccionar
seleccionar:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq %rdi,-8(%rbp)
 movq %rsi,-16(%rbp)
 movq -8(%rbp), %rax
 pushq %rax
 movq -16(%rbp), %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 seta %al
 movzbq %al, %rax
 cmpq $0, %rax
 je ifexp_else_1
 movq -8(%rbp), %rax
 jmp ifexp_end_1
ifexp_else_1:
 movq -16(%rbp), %rax
ifexp_end_1:
 jmp .end_seleccionar
.end_seleccionar:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $10, %rax
 movq %rax, %rdi
 movq $25, %rax
 movq %rax, %rsi
call calcular
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 movq %rax, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 movq $100, %rax
 movq %rax, %rdi
 movq $75, %rax
 movq %rax, %rsi
call seleccionar
 movq %rax, -32(%rbp)
 movq -32(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_uint(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq -32(%rbp), %rax
 cvtsi2sdq %rax, %xmm0
 movq %xmm0, %rax
movq %rax, %xmm0
pushq %rax
 movq $50, %rax
 cvtsi2sdq %rax, %xmm0
 movq %xmm0, %rax
movq %rax, %xmm1
pop %rax
movq %rax, %xmm0
 ucomisd %xmm1, %xmm0
 movl $0, %eax
seta %al
 movzbq %al, %rax
 cmpq $0, %rax
 je ifexp_else_2
 movq -24(%rbp), %rax
movq %rax, %xmm0
pushq %rax
 movq -32(%rbp), %rax
 cvtsi2sdq %rax, %xmm0
 movq %xmm0, %rax
movq %rax, %xmm1
pop %rax
movq %rax, %xmm0
 addsd %xmm1, %xmm0
 movq %xmm0, %rax
 jmp ifexp_end_2
ifexp_else_2:
 movq -24(%rbp), %rax
ifexp_end_2:
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
 .double 1.5
.section .note.GNU-stack,"",@progbits
