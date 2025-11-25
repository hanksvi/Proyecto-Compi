.data
print_fmt: .string "%ld\n"
print_fmt_uint: .string "%lu\n"
print_fmt_float: .string "%f\n"
print_fmt_true: .string "true\n"
print_fmt_false: .string "false\n"
f5: .quad 0
e5: .long 0
c5: .quad 0
b5: .long 0
d5: .long 0
a5: .quad 0
xuint: .quad 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $200, %rax
 movq %rax, xuint(%rip)
 movq xuint(%rip), %rax
 movq %rax, a5(%rip)
 movq xuint(%rip), %rax
 movl %eax, b5(%rip)
 movq xuint(%rip), %rax
 cvtsi2sd %rax, %xmm0
 movq %xmm0, %rax
 movq %rax, c5(%rip)
 movq xuint(%rip), %rax
 cvtsi2ss %rax, %xmm0
 movd %xmm0, %eax
 movl %eax, %eax
 movl %eax, d5(%rip)
 movq xuint(%rip), %rax
 movl %eax, %eax
 movl %eax, e5(%rip)
 movq xuint(%rip), %rax
 cmpq $0, %rax
 movl $0, %eax
 setne %al
 movzbq %al, %rax
 movq %rax, f5(%rip)
 movq xuint(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_uint(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq a5(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl b5(%rip), %eax
 movslq %eax, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq c5(%rip), %rax
 movq %rax, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 movl d5(%rip), %eax
 movd %eax, %xmm0
 cvtss2sd %xmm0, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 movl e5(%rip), %eax
 movl %eax, %eax
 movq %rax, %rsi
 leaq print_fmt_uint(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq f5(%rip), %rax
 movzbq %al, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $0, %eax
 leave
 ret
.section .note.GNU-stack,"",@progbits
