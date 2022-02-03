.cpu cortex-m0plus
.thumb
.syntax unified

.global delay
delay:
        movs r1, #6     // 1 cycle
loop:   subs r1, #1     // 1 cycle
        bne loop        // 3 cycles
        nop             // 1 cycle
        nop             // 1 cycle
        nop             // 1 cycle
        subs r0, #1     // 1 cycle
        bne  delay      // 3 cycles
        bx   lr         // 1 cycle

        // Total delay = r0 * (1 + 6*(1+3) + 1 + 1 + 1 + 1 + 3) + 1