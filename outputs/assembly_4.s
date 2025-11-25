.data
print_fmt: .string "%ld\n"
print_fmt_uint: .string "%lu\n"
print_fmt_float: .string "%f\n"
print_fmt_true: .string "true\n"
print_fmt_false: .string "false\n"
suma: .quad 0
resultado: .quad 0
contador: .quad 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $0, %rax
 movq %rax, contador(%rip)
 movq contador(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq $30, %rax
 movq %rax, suma(%rip)
 movq suma(%rip), %rax
 pushq %rax
 movq $2, %rax
 movq %rax, %rcx
 popq %rax
 imull %ecx
 movq %rax, suma(%rip)
 movq suma(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movsd const_0(%rip), %xmm0
 movq %xmm0, %rax
 movq %rax, resultado(%rip)
 movq resultado(%rip), %rax
 movq %rax, %xmm0
 movq $1, %rax
 leaq print_fmt_float(%rip), %rdi
 call printf@PLT
 movl $0, %eax
 leave
 ret
.section .rodata
.align 8
const_0:
 .double 22
.section .note.GNU-stack,"",@progbits
