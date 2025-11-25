.data
print_fmt: .string "%ld\n"
print_fmt_uint: .string "%lu\n"
print_fmt_float: .string "%f\n"
print_fmt_true: .string "true\n"
print_fmt_false: .string "false\n"
f4: .quad 0
e4: .long 0
d4: .quad 0
c4: .quad 0
b4: .long 0
a4: .quad 0
xfloat32: .long 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movsd float_const_0(%rip), %xmm0
 movq %xmm0, %rax
 movq %rax, %xmm0
 cvtsd2ss %xmm0, %xmm0
 movd %xmm0, %eax
 movl %eax, %eax
 movl %eax, xfloat32(%rip)
 movl xfloat32(%rip), %eax
 movd %eax, %xmm0
 cvttss2si %xmm0, %rax
 movq %rax, a4(%rip)
 movl xfloat32(%rip), %eax
 movd %eax, %xmm0
 cvttss2si %xmm0, %eax
 movl %eax, b4(%rip)
 movl xfloat32(%rip), %eax
 movd %eax, %xmm0
 cvtss2sd %xmm0, %xmm0
 movq %xmm0, %rax
 movq %rax, c4(%rip)
 movl xfloat32(%rip), %eax
 movd %eax, %xmm0
 cvttss2si %xmm0, %rax
 movq %rax, d4(%rip)
 movl xfloat32(%rip), %eax
 movd %eax, %xmm0
 cvttss2si %xmm0, %eax
 movl %eax, e4(%rip)
 movl xfloat32(%rip), %eax
 movd %eax, %xmm0
 xorps %xmm1, %xmm1
 ucomiss %xmm1, %xmm0
 movl $0, %eax
 setne %al
 movzbq %al, %rax
 movq %rax, f4(%rip)
 movl xfloat32(%rip), %eax
 movd %eax, %xmm0
 cvtss2sd %xmm0, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 movq a4(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl b4(%rip), %eax
 movslq %eax, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq c4(%rip), %rax
 movq %rax, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 movq d4(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_uint(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl e4(%rip), %eax
 movl %eax, %eax
 movq %rax, %rsi
 leaq print_fmt_uint(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq f4(%rip), %rax
 movzbq %al, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $0, %eax
 leave
 ret
.section .rodata
.align 8
float_const_0:
 .double 15.5
.section .note.GNU-stack,"",@progbits
