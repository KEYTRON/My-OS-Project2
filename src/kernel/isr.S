.section .text
.global load_idt

load_idt:
    movl 4(%esp), %eax
    lidt (%eax)
    ret
