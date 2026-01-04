.include "macros.inc"

@ void flash_memcpy(volatile unsigned char *dst, const volatile unsigned char *src, size_t size) 
ARM_FUNC_START _flash_memcpy
_flash_memcpy:
1:
	ldrb r3, [r1], #1
	strb r3, [r0], #1
	subs r2, #1
	bne 1b
	bx lr

@ static unsigned int flash_absmemcmp(const volatile unsigned char *dst, const volatile unsigned char *src, size_t size)
ARM_FUNC_START _flash_absmemcmp
_flash_absmemcmp:
1:
	ldrb r3, [r1], #1
	ldrb ip, [r0], #1
	subs r3, r3, ip
	it ne
	movne r0, #1
	bxne lr
	subs r2, #1
	bne 1b
	mov r0, #0
	bx lr
