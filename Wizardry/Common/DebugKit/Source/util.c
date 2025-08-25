#include "prelude.h"
#include "debug-kit.h"

/**
 * No$GBA print
 */
#define NOCASHGBAPRINTADDR 0x4FFFA18

void nocashgba_print(const char *buf)
{
	*(volatile u32 *)NOCASHGBAPRINTADDR = (u32)buf;
}

/**
 * MGBA print
 */
#define REG_DEBUG_ENABLE ((vu16 *) 0x4FFF780)
#define REG_DEBUG_FLAGS ((vu16 *) 0x4FFF700)
#define REG_DEBUG_STRING ((char *) 0x4FFF600)

void mgba_print(const char *buf)
{
	strcpy(REG_DEBUG_STRING, buf);

	mgba_print_level &= 0x7;
	*REG_DEBUG_FLAGS = mgba_print_level | 0x100;
}

bool mgba_open(void)
{
	mgba_print_level = MGBA_LOG_INFO;
	*REG_DEBUG_ENABLE = 0xC0DE;
	return *REG_DEBUG_ENABLE == 0x1DEA;
}

void mgba_close(void)
{
	*REG_DEBUG_ENABLE = 0;
}
