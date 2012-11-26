.intel_syntax noprefix

.extern syscall_interrupt

.global syscall_routine
syscall_routine:
    cli
    push eax
    push 0x80
    pusha
    mov eax, esp
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call syscall_interrupt
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    add esp, 4
    popa
    add esp, 8
    iret

