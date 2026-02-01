#include "macros.inc"
.set UCODE_SIZE, 0x800

.section .text

.balign 16
glabel gspF3DEX3_fifoTextStart
    .incbin "lib/PR/f3dex3/F3DEX3_B_BrZ.code"
glabel gspF3DEX3_fifoTextEnd

.balign 16
glabel gspF3DEX3_CR1_fifoTextStart
    .incbin "lib/PR/f3dex3/F3DEX3_BrZ_NOC_CR1.code"
glabel gspF3DEX3_CR1_fifoTextEnd

/* DATA SECTION START */

.section .data

.balign 16
glabel gspF3DEX3_fifoDataStart
    .incbin "lib/PR/f3dex3/F3DEX3_B_BrZ.data"
glabel gspF3DEX3_fifoDataEnd
