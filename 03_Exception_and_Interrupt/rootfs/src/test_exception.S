.section ".text._start"
.global _start
_start:
    mov x0, 0
loop:
    add x0, x0, 1
    svc 0
    cmp x0, 5
    blt loop

    svc 93 // exit
