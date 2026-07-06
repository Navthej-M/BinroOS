.section .text

# no ec
.global isr0
isr0:
    pushq           $0
    pushq           $0
    jmp             common_stub

# no ec
.global isr1
isr1:
    pushq           $0
    pushq           $1
    jmp             common_stub

# no ec
.global isr2
isr2:
    pushq           $0
    pushq           $2
    jmp             common_stub

# no ec
.global isr3
isr3:
    pushq           $0
    pushq           $3
    jmp             common_stub

# no ec
.global isr4
isr4:
    pushq           $0
    pushq           $4
    jmp             common_stub

# no ec
.global isr5
isr5:
    pushq           $0
    pushq           $5
    jmp             common_stub

# no ec
.global isr6
isr6:
    pushq           $0
    pushq           $6
    jmp             common_stub

# no ec
.global isr7
isr7:
    pushq           $0
    pushq           $7
    jmp             common_stub

# ec
.global isr8
isr8:
    pushq           $8
    jmp             common_stub

# ec
.global isr10
isr10:
    pushq           $10
    jmp             common_stub

# ec
.global isr11
isr11:
    pushq           $11
    jmp             common_stub

# ec
.global isr12
isr12:
    pushq           $12
    jmp             common_stub

# ec
.global isr13
isr13:
    pushq           $13
    jmp             common_stub

# ec
.global isr14
isr14:
    pushq           $14
    jmp             common_stub

# no ec
.global isr16
isr16:
    pushq           $16
    jmp             common_stub

# ec
.global isr17
isr17:
    pushq           $17
    jmp             common_stub

# no ec
.global isr18
isr18:
    pushq           $0
    pushq           $18
    jmp             common_stub

# no ec
.global isr19
isr19:
    pushq           $0
    pushq           $19
    jmp             common_stub

# no ec
.global isr20
isr20:
    pushq           $0
    pushq           $20
    jmp             common_stub

# ec
.global isr21
isr21:
    pushq           $21
    jmp             common_stub

# no ec
.global isr28
isr28:
    pushq           $28
    jmp             common_stub

# ec
.global isr29
isr29:
    pushq           $29
    jmp             common_stub

# ec
.global isr30
isr30:
    pushq           $30
    jmp             common_stub

# no ec
.global isr33
isr33:
    pushq           $0
    pushq           $33
    jmp             common_stub

.extern isr_common_interrupt_dispatcher

.type idt_isr_common, @function
common_stub:
    pushq            %rax
    pushq            %rbx
    pushq            %rcx
    pushq            %rdx
    pushq            %rsi
    pushq            %rdi
    pushq            %rbp
    pushq            %r8
    pushq            %r9
    pushq            %r10
    pushq            %r11
    pushq            %r12
    pushq            %r13
    pushq            %r14
    pushq            %r15

    movq            %rsp, %rdi
    call            isr_common_interrupt_dispatcher

    popq            %r15
    popq            %r14
    popq            %r13
    popq            %r12
    popq            %r11
    popq            %r10
    popq            %r9
    popq            %r8
    popq            %rbp
    popq            %rdi
    popq            %rsi
    popq            %rdx
    popq            %rcx
    popq            %rbx
    popq            %rax

    addq            $16, %rsp           # error_code, and vector
    iretq
