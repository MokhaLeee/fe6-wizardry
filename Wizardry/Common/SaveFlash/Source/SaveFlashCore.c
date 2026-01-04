#include "prelude.h"
#include "save-flash.h"
#include "debug-screen.h"

extern void flash_memcpy(volatile unsigned char *dst, const volatile unsigned char *src, size_t size);

#define MEM_FLASH 0x0E000000
#define FLASH_SIZE 0x10000
#define flash_mem ((vu8*)MEM_FLASH)

#define FLASH_SECTOR_SIZE_4KB 4096 // all device types, except Atmel
#define FLASH_SECTOR_SIZE_128B 128 // only Atmel devices

#define LOOP_CNT_PER_MILLI_SECOND 1000

enum FlashCmd {
	FLASH_CMD_ERASE_CHIP = 1,
	FLASH_CMD_ERASE_SECTOR = 3,
	FLASH_CMD_ERASE = 8,
	FLASH_CMD_ENTER_ID_MODE = 9,
	FLASH_CMD_WRITE = 0xA,
	FLASH_CMD_SWITCH_BANK = 0xB,
	FLASH_CMD_LEAVE_ID_MODE = 0xF,
};

#define FLASH_CMD_BEGIN flash_mem[0x5555] = 0xAA; flash_mem[0x2AAA] = 0x55;
#define FLASH_CMD(cmd) FLASH_CMD_BEGIN; flash_mem[0x5555] = (cmd) << 4;

static const unsigned char erased_byte_value = 0xFF;
static const unsigned char erased_byte_value_vba = 0;

extern struct FlashInfo {
	u8 device;
	u8 manufacturer;
	u8 size;
} gFlashInfo;

static const struct FlashInfo flash_chips[] = {
	{FLASH_DEV_MX29L512, FLASH_MFR_MACRONIX, FLASH_SIZE_64KB},
	{FLASH_DEV_MN63F805MNP, FLASH_MFR_PANASONIC, FLASH_SIZE_64KB},
	{FLASH_DEV_LE39FW512, FLASH_MFR_SST, FLASH_SIZE_64KB},
	{FLASH_DEV_AT29LV512, FLASH_MFR_ATMEL, FLASH_SIZE_64KB},
	{FLASH_DEV_MX29L010, FLASH_MFR_MACRONIX, FLASH_SIZE_128KB},
	{FLASH_DEV_LE26FV10N1TS, FLASH_MFR_SANYO, FLASH_SIZE_128KB},
};

#define FLASH_CHIP_NUM sizeof(flash_chips) / sizeof(flash_chips[0])

// wait until timeout 
static void wait(int timeout)
{
	for (vu32 i = 0; i < LOOP_CNT_PER_MILLI_SECOND * timeout; i++);
}

// wait until [E00xxxxh]=dat (or timeout)
static int wait_until(u32 addr, const u8 *data, int timeout)
{
	for (vu32 i = 0; i < LOOP_CNT_PER_MILLI_SECOND * timeout && flash_absmemcmp(&flash_mem[addr], data, 1); i++);

	if (flash_absmemcmp(&flash_mem[addr], data, 1)) {
		// Terminate Command after Timeout (only Macronix devices, ID=1CC2h)
		if (gFlashInfo.manufacturer == FLASH_MFR_MACRONIX && gFlashInfo.device == FLASH_DEV_MX29L512)
			// force end of write/erase command
			flash_mem[0x5555] = 0xF0;

		return -1;
	}

	return 0;
}
