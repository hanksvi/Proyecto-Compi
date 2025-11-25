.data
print_fmt: .string "%ld\n"
print_fmt_uint: .string "%lu\n"
print_fmt_float: .string "%f\n"
print_fmt_true: .string "true\n"
print_fmt_false: .string "false\n"
resultado: .quad 0
z: .quad 0
y: .quad 0
x: .quad 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $65, %rax
 movq %rax, x(%rip)
 movq x(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movsd const_0(%rip), %xmm0
 movq %xmm0, %rax
 movq %rax, y(%rip)
 movq y(%rip), %rax
 movq %rax, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 movq $275, %rax
 movq %rax, z(%rip)
 movq z(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt_uint(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $7, %rax
 movq %rax, resultado(%rip)
 movq resultado(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $0, %eax
 leave
 ret
.section .rodata
.align 8
const_0:
 .double 15
.section .note.GNU-stack,"",@progbits
