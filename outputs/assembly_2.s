.data
print_fmt: .string "%ld \n"
y: .quad 0
x: .quad 0
.text
.globl main
main:
 pushq %rbp
 movq %rsp, %rbp
 movq $0, %rax
 movq %rax, x(%rip)
 movq x(%rip), %rax
 cmpq $0, %rax
 movl $0, %eax
 setne %al
 movzbq %al, %rax
 movq %rax, y(%rip)
 movq y(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movl $0, %eax
 leave
 ret
.section .note.GNU-stack,"",@progbits
