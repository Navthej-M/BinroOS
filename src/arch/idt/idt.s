.section .text

.global idt_load
.type idt_load, @function
idt_load:
    lidtq            (%rdi)
    ret
