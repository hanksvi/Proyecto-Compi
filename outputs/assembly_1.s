.data
print_fmt: .string "%ld \n"
mensaje: .quad 0
y: .quad 0
x: .quad 0
.text
.globl suma
suma:
 pushq %rbp
 movq %rsp, %rbp
 movq %rdi,-8(%rbp)
 movq %rsi,-16(%rbp)
 subq $16, %rsp
 movq x(%rip), %rax
 cmpq $0, %rax
 je else_0
 movq y(%rip), %rax
 cmpq $0, %rax
 je else_1
 movq y(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp endif_1
 else_1:
endif_1:
 movq x(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
 jmp endif_0
 else_0:
endif_0:
 movq x(%rip), %rax
 pushq %rax
 movq y(%rip), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 jmp .end_suma
.end_suma:
leave
ret
.section .note.GNU-stack,"",@progbits
