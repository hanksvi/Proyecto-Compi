.data
print_fmt: .string "%ld\n"
print_fmt_uint: .string "%lu\n"
print_fmt_float: .string "%f\n"
print_fmt_true: .string "true\n"
print_fmt_false: .string "false\n"
.text
.globl factorialUint
factorialUint:
 pushq %rbp
 movq %rsp, %rbp
 subq $8, %rsp
 movq %rdi,-8(%rbp)
 movq -8(%rbp), %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setbe %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_0
 movq $1, %rax
 jmp .end_factorialUint
 jmp endif_0
 else_0:
 movq -8(%rbp), %rax
 pushq %rax
 movq -8(%rbp), %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 subl %ecx, %eax
 mov %rax, %rdi
call factorialUint
 movq %rax, %rcx
 popq %rax
 mull %ecx
 jmp .end_factorialUint
endif_0:
.end_factorialUint:
leave
ret
.globl potencia
potencia:
 pushq %rbp
 movq %rsp, %rbp
 subq $16, %rsp
 movq %rdi,-8(%rbp)
 movq %rsi,-16(%rbp)
 movq -16(%rbp), %rax
 pushq %rax
 movq $0, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 sete %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_1
 movq $1, %rax
 cvtsi2sd %rax, %xmm0
 movq %xmm0, %rax
 jmp .end_potencia
 jmp endif_1
 else_1:
 movq -8(%rbp), %rax
movq %rax, %xmm0
pushq %rax
 movq -8(%rbp), %rax
 mov %rax, %rdi
 movq -16(%rbp), %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 subl %ecx, %eax
 mov %rax, %rsi
call potencia
movq %rax, %xmm1
pop %rax
movq %rax, %xmm0
 mulsd %xmm1, %xmm0
 movq %xmm0, %rax
 jmp .end_potencia
endif_1:
.end_potencia:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $5, %rax
 mov %rax, %rdi
call factorialUint
 movq %rax, -24(%rbp)
 movq -24(%rbp), %rax
 movq %rax, %rsi
 leaq print_fmt_uint(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movsd float_const_0(%rip), %xmm0
 movq %xmm0, %rax
 mov %rax, %rdi
 movq $3, %rax
 mov %rax, %rsi
call potencia
 movq %rax, -32(%rbp)
 movq -32(%rbp), %rax
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
 .double 2.5
.section .note.GNU-stack,"",@progbits
