.data
print_fmt: .string "%ld \n"
res: .quad 0
y: .quad 0
x: .quad 0
.text
 movq $5, %rax
 movq %rax, x(%rip)
 movq $12, %rax
 movq %rax, y(%rip)
 movq x(%rip), %rax
 pushq %rax
 movq y(%rip), %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setle %al
 movzbq %al, %rax
 cmpq $0, %rax
 je else_0
 movq x(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq y(%rip), %rax
 movq %rax, res(%rip)
 jmp endif_0
 else_0:
 movq y(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 movq x(%rip), %rax
 movq %rax, res(%rip)
endif_0:
 movq res(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.section .note.GNU-stack,"",@progbits
