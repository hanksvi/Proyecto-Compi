.data
print_fmt: .string "%ld\n"
print_fmt_uint: .string "%lu\n"
print_fmt_float: .string "%f\n"
print_fmt_true: .string "true\n"
print_fmt_false: .string "false\n"
y: .quad 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movsd float_const_0(%rip), %xmm0
 movq %xmm0, %rax
 movq %rax, y(%rip)
 movq y(%rip), %rax
movq %rax, %xmm0
pushq %rax
 movsd float_const_1(%rip), %xmm0
 movq %xmm0, %rax
movq %rax, %xmm1
pop %rax
movq %rax, %xmm0
 mulsd %xmm1, %xmm0
 movq %xmm0, %rax
 movq %rax, y(%rip)
 movq y(%rip), %rax
 movq %rax, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 movq $0, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq y(%rip), %rax
movq %rax, %xmm0
pushq %rax
 movsd float_const_2(%rip), %xmm0
 movq %xmm0, %rax
movq %rax, %xmm1
pop %rax
movq %rax, %xmm0
 addsd %xmm1, %xmm0
 movq %xmm0, %rax
 movq %rax, y(%rip)
 movq y(%rip), %rax
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
 .double 10.5
float_const_1:
 .double 2
float_const_2:
 .double 5.5
.section .note.GNU-stack,"",@progbits
