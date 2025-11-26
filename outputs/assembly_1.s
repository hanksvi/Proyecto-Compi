.data
print_fmt: .string "%ld\n"
print_fmt_uint: .string "%lu\n"
print_fmt_float: .string "%f\n"
print_fmt_true: .string "true\n"
print_fmt_false: .string "false\n"
.text
.globl calcularConstante
calcularConstante:
 pushq %rbp
 movq %rsp, %rbp
 subq $8, %rsp
 movq $200, %rax
 movq %rax, -8(%rbp)
 movq -8(%rbp), %rax
 pushq %rax
 movq $50, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, -8(%rbp)
 movq -8(%rbp), %rax
 jmp .end_calcularConstante
.end_calcularConstante:
leave
ret
.globl evaluarCondiciones
evaluarCondiciones:
 pushq %rbp
 movq %rsp, %rbp
 subq $0, %rsp
 movq $1, %rax
 jmp .end_evaluarCondiciones
.end_evaluarCondiciones:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
call calcularConstante
 movq %rax, -8(%rbp)
 movq -8(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
call evaluarCondiciones
 movq %rax, -16(%rbp)
 movq -16(%rbp), %rax
 movzbq %al, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movsd const_0(%rip), %xmm0
 movq %xmm0, %rax
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 movq %rax, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 movq -16(%rbp), %rax
 cmpq $0, %rax
 je else_0
 movq -24(%rbp), %rax
movq %rax, %xmm0
pushq %rax
 movsd float_const_1(%rip), %xmm0
 movq %xmm0, %rax
movq %rax, %xmm1
pop %rax
movq %rax, %xmm0
 addsd %xmm1, %xmm0
 movq %xmm0, %rax
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 movq %rax, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 jmp endif_0
 else_0:
endif_0:
 movl $0, %eax
 leave
 ret
.section .rodata
.align 8
const_0:
 .double 26
float_const_1:
 .double 10
.section .note.GNU-stack,"",@progbits
