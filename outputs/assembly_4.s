.data
print_fmt: .string "%ld \n"
suma: .quad 0
i: .quad 0
.text
 movq $0, %rax
 movq %rax, i(%rip)
 movq $0, %rax
 movq %rax, suma(%rip)
while_0:
 movq i(%rip), %rax
 pushq %rax
 movq $5, %rax
 movq %rax, %rcx
 popq %rax
 cmpq %rcx, %rax
 movl $0, %eax
 setle %al
 movzbq %al, %rax
 cmpq $0, %rax
 je endwhile_0
 movq suma(%rip), %rax
 pushq %rax
 movq i(%rip), %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, suma(%rip)
 movq i(%rip), %rax
 pushq %rax
 movq $1, %rax
 movq %rax, %rcx
 popq %rax
 addq %rcx, %rax
 movq %rax, i(%rip)
 jmp while_0
endwhile_0:
 movq suma(%rip), %rax
 movq %rax, %rsi
 leaq print_fmt(%rip), %rdi
 movl $0, %eax
 call printf@PLT
.section .note.GNU-stack,"",@progbits
