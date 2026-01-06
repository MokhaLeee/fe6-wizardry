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


SET_DATA FreeRamSpaceTop,    0x02025B8C
SET_DATA FreeRamSpaceBottom, 0x02027BB4
SET_DATA UsedFreeRamSpaceTop, FreeRamSpaceBottom

.macro _kernel_malloc name, size
    .set UsedFreeRamSpaceTop, UsedFreeRamSpaceTop - \size
    SET_DATA \name, UsedFreeRamSpaceTop
.endm

_kernel_malloc stdio_init_magic, 4
_kernel_malloc mgba_print_level, 4
_kernel_malloc __stdio_FILEs, 0x10
_kernel_malloc gFlashInfo, 0x10

/**
 * IWRAM
 * Free: 03003578 - 03003780
 */
SET_DATA ARM_flash_memcpy, 0x03003578
SET_DATA ARM_flash_absmemcmp, 0x0300358C
