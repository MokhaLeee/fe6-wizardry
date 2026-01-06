#include "prelude.h"
#include "gbasram.h"
#include "debugtext.h"

#include "save-flash.h"
#include "debug-kit.h"
#include "debug-screen.h"

// extern void flash_memcpy(volatile unsigned char *dst, const volatile unsigned char *src, size_t size);
// extern unsigned int flash_absmemcmp(const volatile unsigned char *dst, const volatile unsigned char *src, size_t size);

#define flash_memcpy(dst, src, size) ReadSramFast(src, dst, size)

#define MEM_FLASH 0x0E000000
#define FLASH_SIZE 0x10000
#define flash_mem ((u8*)MEM_FLASH)

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

__unused
static const unsigned char erased_byte_value = 0xFF;
__unused
static const unsigned char erased_byte_value_vba = 0;

extern struct FlashInfo {
	u8 device;
	u8 manufacturer;
	u8 size;
} gFlashInfo;

__unused
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
	for (int i = 0; i < LOOP_CNT_PER_MILLI_SECOND * timeout; i++);
}

static int flash_absmemcmp(const void *mem_flash, const void *mem_ram, int size)
{
	int i;
	u8 buf[size];
	const u8 *mem = mem_ram;

	flash_memcpy(buf, mem_flash, size);

	for (i = 0; i < size; i++)
		if (mem[i] != buf[i])
			return 1;

	return 0;
}

// wait until [E00xxxxh]=dat (or timeout)
__unused
static int wait_until(u32 addr, const u8 *data, int timeout)
{
	int i, ret = 0;

	for (i = 0; i < (LOOP_CNT_PER_MILLI_SECOND * timeout); i++) {
		ret = flash_absmemcmp(&flash_mem[addr], data, 1);
		if (ret == 0)
			break;
	}

	if (ret) {
		// Terminate Command after Timeout (only Macronix devices, ID=1CC2h)
		if (gFlashInfo.manufacturer == FLASH_MFR_MACRONIX && gFlashInfo.device == FLASH_DEV_MX29L512)
			// force end of write/erase command
			flash_mem[0x5555] = 0xF0;

		return -1;
	}
	return 0;
}

__unused
void debug_flash_info(void)
{
	char buf[0x100];

	snprintf(buf, 0x100, "device=%d\n", gFlashInfo.device);
	DebugPrintStr(buf);

	snprintf(buf, 0x100, "manufacturer=%d\n", gFlashInfo.manufacturer);
	DebugPrintStr(buf);

	snprintf(buf, 0x100, "size=%d\n", gFlashInfo.size);
	DebugPrintStr(buf);

	StartDebugScreen();
}

// Chip Identification (all device types)
int flash_init(u8 size)
{
	// Use 8 clk waitstates for initial detection (WAITCNT Bits 0,1 both set). After detection of certain device types smaller wait values may be used for write/erase, and even smaller wait values for raw reading, see Device Types table.
	// REG_WAITCNT |= WS_SRAM_8;
	REG_WAITCNT = (REG_WAITCNT & ~3) | 3;

	// enter ID mode
	FLASH_CMD(FLASH_CMD_ENTER_ID_MODE);

	// one minor thing the atmel docs say: you have to wait 20ms when entering or exiting ID mode.
	wait(20);

	// get device & manufacturer
	flash_memcpy(&gFlashInfo.device, &flash_mem[1], 1);
	flash_memcpy(&gFlashInfo.manufacturer, &flash_mem[0], 1);

	// terminate ID mode
	FLASH_CMD(FLASH_CMD_LEAVE_ID_MODE);

	// one minor thing the atmel docs say: you have to wait 20ms when entering or exiting ID mode.
	wait(20);

	// 128K sanyo flash needs to have the "exit ID mode" written TWICE to work. If you only write it once, it will not exit ID mode.
	// 64K sanyo flash has the same device/manufacturer ID as the SST part.
	if (gFlashInfo.manufacturer == FLASH_MFR_SANYO)
		flash_mem[0x5555] = FLASH_CMD_LEAVE_ID_MODE << 4;

	gFlashInfo.size = 0;

	for (u32 i = 0; i < FLASH_CHIP_NUM; i++) {
		if (gFlashInfo.manufacturer == flash_chips[i].manufacturer && gFlashInfo.device == flash_chips[i].device) {
			gFlashInfo.size = flash_chips[i].size;
		}
	}

	if (size)
		gFlashInfo.size = size;

	debug_flash_info();
	
	if (!gFlashInfo.size)
		return -1;

	return 0;
}
