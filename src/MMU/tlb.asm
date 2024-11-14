global tlb_flush
tlb_flush:
    mov cr3 eax
    mov eax cr3
    ret
