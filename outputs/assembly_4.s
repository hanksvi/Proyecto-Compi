.data
print_fmt: .string "%ld \n"
flag: .quad 0
x: .quad 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $10, %rax
 movq %rax, x(%rip)
 movq x(%rip), %rax
 cmpq $0, %rax
 movl $0, %eax
 setne %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_0
 movq $1, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp endif_0
 else_0:
 movq $0, %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
endif_0:
 movl $0, %eax
 leave
 ret
.section .note.GNU-stack,"",@progbits
