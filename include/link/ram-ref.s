.macro SET_DATA name, value
    .global \name
    .type \name, object
    .set \name, \value
.endm

.macro dat value, name
    .global \name
    .type \name, object
    .set \name, \value
.endm

SET_DATA FreeRamSpaceTop,    0x0203F150
SET_DATA FreeRamSpaceBottom, 0x02040000
SET_DATA UsedFreeRamSpaceTop, FreeRamSpaceBottom

.macro _kernel_malloc name, size
    .set UsedFreeRamSpaceTop, UsedFreeRamSpaceTop - \size
    SET_DATA \name, UsedFreeRamSpaceTop
.endm

_kernel_malloc stdio_init_magic, 4
_kernel_malloc mgba_print_level, 4
_kernel_malloc __stdio_FILEs, 0x10
