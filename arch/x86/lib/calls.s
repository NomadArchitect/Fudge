global call_open
call_open:
    push ebp
    mov ebp, esp
    push esi
    push edi
    mov eax, 0x00
    mov esi, [ebp + 8]
    int 0x80
    pop edi
    pop esi
    pop ebp
    ret

global call_execute
call_execute:
    push ebp
    mov ebp, esp
    push esi
    push edi
    mov eax, 0x04
    mov ebx, [ebp + 8]
    mov ecx, [ebp + 12]
    mov esi, [ebp + 13]
    int 0x80
    pop edi
    pop esi
    pop ebp
    ret

global call_load
call_load:
    push ebp
    mov ebp, esp
    push ebx
    push esi
    push edi
    mov eax, 0x05
    mov ebx, [ebp + 8]
    int 0x80
    pop edi
    pop esi
    pop ebx
    pop ebp
    ret

global call_unload
call_unload:
    push ebp
    mov ebp, esp
    push esi
    push edi
    mov eax, 0x06
    int 0x80
    pop edi
    pop esi
    pop ebp
    ret

global call_halt
call_halt:
    push ebp
    mov ebp, esp
    push esi
    push edi
    mov eax, 0x20
    int 0x80
    pop edi
    pop esi
    pop ebp
    ret

global call_reboot
call_reboot:
    push ebp
    mov ebp, esp
    push esi
    push edi
    mov eax, 0x21
    int 0x80
    pop edi
    pop esi
    pop ebp
    ret

