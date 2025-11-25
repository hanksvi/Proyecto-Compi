.data
print_fmt: .string "%ld\n"
print_fmt_uint: .string "%lu\n"
print_fmt_float: .string "%f\n"
print_fmt_true: .string "true\n"
print_fmt_false: .string "false\n"
e6: .quad 0
f6: .quad 0
d6: .long 0
c6: .quad 0
a6: .quad 0
b6: .long 0
xuint32: .long 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $150, %rax
 movl %eax, %eax
 movl %eax, xuint32(%rip)
 movl xuint32(%rip), %eax
 movl %eax, %eax
 movq %rax, a6(%rip)
 movl xuint32(%rip), %eax
 movl %eax, b6(%rip)
 movl xuint32(%rip), %eax
 movl %eax, %eax
 cvtsi2sdq %rax, %xmm0
 movq %xmm0, %rax
 movq %rax, c6(%rip)
 movl xuint32(%rip), %eax
 movl %eax, %eax
 cvtsi2ssq %rax, %xmm0
 movd %xmm0, %eax
 movl %eax, %eax
 movl %eax, d6(%rip)
 movl xuint32(%rip), %eax
 movl %eax, %eax
 movq %rax, e6(%rip)
 movl xuint32(%rip), %eax
 cmpl $0, %eax
 movl $0, %eax
 setne %al
 movzbq %al, %rax
 movq %rax, f6(%rip)
 movl xuint32(%rip), %eax
 movq %rax, %rsi
 leaq print_fmt_uint(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq a6(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl b6(%rip), %eax
 movslq %eax, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq c6(%rip), %rax
 movq %rax, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 movl d6(%rip), %eax
 movd %eax, %xmm0
 cvtss2sd %xmm0, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 movq e6(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_uint(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq f6(%rip), %rax
 movzbq %al, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $0, %eax
 leave
 ret
.section .note.GNU-stack,"",@progbits
