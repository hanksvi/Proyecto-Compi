.data
print_fmt: .string "%ld\n"
print_fmt_uint: .string "%lu\n"
print_fmt_float: .string "%f\n"
print_fmt_true: .string "true\n"
print_fmt_false: .string "false\n"
x: .quad 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $0, %rax
 movq %rax, x(%rip)
 movq $100, %rax
 movq %rax, x(%rip)
 movq x(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq x(%rip), %rax
 pushq %rax
 movq $50, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, x(%rip)
 movq x(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $1, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $0, %eax
 leave
 ret
.section .note.GNU-stack,"",@progbits
