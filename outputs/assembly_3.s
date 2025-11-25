.data
print_fmt: .string "%ld\n"
print_fmt_uint: .string "%lu\n"
print_fmt_float: .string "%f\n"
print_fmt_true: .string "true\n"
print_fmt_false: .string "false\n"
.text
.globl operacionCompleja
operacionCompleja:
 pushq %rbp
 movq %rsp, %rbp
 subq $24, %rsp
 movq %rdi,-8(%rbp)
 movq %rsi,-12(%rbp)
 movq %rdx,-16(%rbp)
 movl -8(%rbp), %eax
 cvtsi2sd %eax, %xmm0
 movq %xmm0, %rax
movq %rax, %xmm0
pushq %rax
 movl -12(%rbp), %eax
 movd %eax, %xmm0
 cvtss2sd %xmm0, %xmm0
 movq %xmm0, %rax
movq %rax, %xmm1
pop %rax
movq %rax, %xmm0
 addsd %xmm1, %xmm0
 movq %xmm0, %rax
 movq %rax, -20(%rbp)
 movl -16(%rbp), %eax
 cvtsi2sd %eax, %xmm0
 movq %xmm0, %rax
movq %rax, %xmm0
pushq %rax
 movq $2, %rax
 cvtsi2sd %rax, %xmm0
 movq %xmm0, %rax
movq %rax, %xmm1
pop %rax
movq %rax, %xmm0
 mulsd %xmm1, %xmm0
 movq %xmm0, %rax
 movq %rax, -28(%rbp)
 movq -20(%rbp), %rax
movq %rax, %xmm0
pushq %rax
 movq -28(%rbp), %rax
movq %rax, %xmm1
pop %rax
movq %rax, %xmm0
 addsd %xmm1, %xmm0
 movq %xmm0, %rax
 jmp .end_operacionCompleja
.end_operacionCompleja:
leave
ret
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $25, %rax
 movl %eax, -44(%rbp)
 movq $10, %rax
 movl %eax, -36(%rbp)
 movsd float_const_0(%rip), %xmm0
 movq %xmm0, %rax
 movq %rax, %xmm0
 cvtsd2ss %xmm0, %xmm0
 movd %xmm0, %eax
 movl %eax, %eax
 movl %eax, -40(%rbp)
 movl -36(%rbp), %eax
 movslq %eax, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl -40(%rbp), %eax
 movd %eax, %xmm0
 cvtss2sd %xmm0, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 movl -44(%rbp), %eax
 movl %eax, %eax
 movq %rax, %rsi
 leaq print_fmt_uint(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl -36(%rbp), %eax
 mov %rax, %rdi
 movl -40(%rbp), %eax
 mov %rax, %rsi
 movl -44(%rbp), %eax
 mov %rax, %rdx
call operacionCompleja
 movq %rax, -48(%rbp)
 movq -48(%rbp), %rax
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
 .double 3.5
.section .note.GNU-stack,"",@progbits
