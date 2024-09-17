MAKEFLAGS += --no-print-directory

MK_PATH   := $(abspath $(lastword $(MAKEFILE_LIST)))
MK_DIR    := $(dir $(MK_PATH))

MAIN    := $(MK_DIR)main.event
FE6_CHX := $(MK_DIR)fe6-chax.gba
FE6_GBA := $(MK_DIR)fe6.gba

TOOL_DIR := $(MK_DIR)Tools
LIB_DIR  := $(TOOL_DIR)/FE6-CLib
FE6_REF  := $(LIB_DIR)/reference/fe6.ref.o
FE6_SYM  := $(LIB_DIR)/reference/fe6.sym

WIZARDRY_DIR := $(MK_DIR)Wizardry
HACK_DIRS := $(WIZARDRY_DIR)

all:
	@$(MAKE) pre_build	|| exit 1
	@$(MAKE) chax		|| exit 1
	@$(MAKE) post_chax	|| exit 1

CACHE_DIR := $(MK_DIR).cache_dir
$(shell mkdir -p $(CACHE_DIR) > /dev/null)

CLEAN_FILES :=
CLEAN_DIRS  := $(CACHE_DIR) .release_dir $(shell find -name __pycache__)
CLEAN_BUILD :=

# =========
# = Tools =
# =========

ifeq ($(OS),Windows_NT)
	$(error "need linux environment, abort)
endif

ifeq ($(strip $(DEVKITPRO)),)
	$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitpro)
endif

ifeq ($(strip $(DEVKITARM)),)
	$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM)
endif

TOOLCHAIN ?= $(DEVKITARM)
ifneq (,$(TOOLCHAIN))
	export PATH := $(TOOLCHAIN)/bin:$(PATH)
endif

ifeq ($(CONFIG_LONG_CALL), 1)
	LYN_LONG_CALL := -longcalls
	GCC_LONG_CALL := -mlong-calls
else
	LYN_LONG_CALL :=
	GCC_LONG_CALL :=
endif

PREFIX  ?= arm-none-eabi-
CC      := $(PREFIX)gcc
AS      := $(PREFIX)as
OBJCOPY := $(PREFIX)objcopy

EA_DIR            := $(TOOL_DIR)/EventAssembler
EA                := $(EA_DIR)/ColorzCore
PARSEFILE         := $(EA_DIR)/Tools/ParseFile
PNG2DMP           := $(EA_DIR)/Tools/Png2Dmp
COMPRESS          := $(EA_DIR)/Tools/compress
LYN               := $(EA_DIR)/Tools/lyn $(LYN_LONG_CALL)
EA_DEP            := $(EA_DIR)/ea-dep

TEXT_PROCESS      := python3 $(TOOL_DIR)/FE-PyTools/text-process-classic.py

LYN_PROTECTOR := $(TOOL_DIR)/scripts/lynjump-protector.sh
LYN_DETECTOR  := $(TOOL_DIR)/scripts/lynjump-detector.sh

GRIT := $(DEVKITPRO)/tools/bin/grit
LZSS := $(DEVKITPRO)/tools/bin/gbalzss

GRITLZ77ARGS      := -gu 16 -gzl -gB 4 -p! -m! -ft bin -fh!
GRIT4BPPARGS      := -gu 16 -gB 4 -p! -m! -ft bin -fh!
GRIT2BPPARGS      := -gu 16 -gb -gB 2 -p! -m! -ft bin -fh!
GRITPALETTEARGS	  := -g! -m! -p -ft bin -fh!
MAPPALETTEARGS    := -pn 160
BTLPALETTEARGS    := -pn 80

# ========
# = Main =
# ========

chax: $(FE6_CHX)
PRE_BUILD :=

EA_FLAG := A FE6 -werr

$(FE6_CHX): $(MAIN) $(FE6_GBA) $(FE6_SYM) $(shell $(EA_DEP) $(MAIN) -I $(EA_DIR) --add-missings)
	@echo "[GEN]	$@"
	@cp -f $(FE6_GBA) $(FE6_CHX)
	@$(EA) $(EA_FLAG) -input:$(MAIN) -output:$(FE6_CHX) --nocash-sym || { rm -f $(FE6_CHX); exit 1; }

CHAX_SYM := $(FE6_CHX:.gba=.sym)
CHAX_REFS := $(FE6_CHX:.gba=.ref.s)
CHAX_REFE := $(FE6_CHX:.gba=.ref.event)
CHAX_DIFF := $(FE6_CHX:.gba=.bsdiff)

POST_CHAX := $(CACHE_DIR)/post_chax.log
post_chax: $(POST_CHAX)

$(POST_CHAX): $(FE6_CHX)
	@echo "[SEC]	Lyn-jump detection..."
	@$(LYN_DETECTOR) || exit 1
	@echo "[SEC]	Lyn-jump detection passed"
	@echo "Done!"
	@echo "Done!" > $(POST_CHAX)

CLEAN_FILES += $(FE6_CHX)  $(CHAX_SYM) $(CHAX_REFS) $(CHAX_REFE) $(CHAX_DIFF)

# ============
# = Wizardry =
# ============

INC_DIRS := include $(LIB_DIR)/include
INC_FLAG := $(foreach dir, $(INC_DIRS), -I $(dir))

ARCH    := -mcpu=arm7tdmi -mthumb -mthumb-interwork
CFLAGS  := $(ARCH) $(INC_FLAG) -Wall -Wextra -Werror -Wno-unused-parameter -O2 -mtune=arm7tdmi $(GCC_LONG_CALL)
ASFLAGS := $(ARCH) $(INC_FLAG)

CDEPFLAGS = -MMD -MT "$*.o" -MT "$*.asm" -MF "$(CACHE_DIR)/$(notdir $*).d" -MP
SDEPFLAGS = --MD "$(CACHE_DIR)/$(notdir $*).d"

LYN_REF := $(EXT_REF:.s=.o) $(RAM_REF:.s=.o) $(FE6_REF)

%.lyn.event: %.o $(LYN_REF) $(FE6_SYM)
	@echo "[LYN]	$@"
	@$(LYN) $< $(LYN_REF) > $@
	@$(LYN_PROTECTOR) $@ $(FE6_SYM) >> $@

%.dmp: %.o
	@echo "[GEN]	$@"
	@$(OBJCOPY) -S $< -O binary $@

%.o: %.s
	@echo "[AS ]	$@"
	@$(AS) $(ASFLAGS) $(SDEPFLAGS) -I $(dir $<) $< -o $@

%.o: %.c
	@echo "[CC ]	$@"
	@$(CC) $(CFLAGS) $(CDEPFLAGS) -g -c $< -o $@

%.asm: %.c
	@echo "[CC ]	$@"
	@$(CC) $(CFLAGS) $(CDEPFLAGS) -S $< -o $@ -fverbose-asm

# Avoid make deleting objects it thinks it doesn't need anymore
# Without this make may fail to detect some files as being up to date
.PRECIOUS: %.o;

-include $(wildcard $(CACHE_DIR)/*.d)

CFILES := $(shell find $(HACK_DIRS) -type f -name '*.c')
CLEAN_FILES += $(CFILES:.c=.o) $(CFILES:.c=.asm) $(CFILES:.c=.dmp) $(CFILES:.c=.lyn.event)

SFILES := $(shell find $(HACK_DIRS) -type f -name '*.s')
CLEAN_FILES += $(SFILES:.s=.o) $(SFILES:.s=.dmp) $(SFILES:.s=.lyn.event)

# ============
# = Spritans =
# ============

PNG_FILES := $(shell find $(HACK_DIRS) -type f -name '*.png')
TSA_FILES := $(shell find $(HACK_DIRS) -type f -name '*.tsa')

%.4bpp: %.png
	@echo "[GEN]	$@"
	@cd $(dir $<) && $(GRIT) $(notdir $<) $(GRIT4BPPARGS)
	@mv $(basename $<).img.bin $@

%.gbapal: %.png
	@echo "[GEN]	$@"
	@cd $(dir $<) && $(GRIT) $(notdir $<) $(GRITPALETTEARGS)
	@mv $(basename $<).pal.bin $@

%.lz: %
	@echo "[LZ ]	$@"
	@$(LZSS) e $< $@

%.lz77: %.png
	@echo "[LZ ]	$@"
	@cd $(dir $<) && $(GRIT) $< $(GRITLZ77ARGS)
	@mv $(basename $<).img.bin $@

CLEAN_FILES += $(PNG_FILES:.png=.gbapal) $(PNG_FILES:.png=.4bpp) $(PNG_FILES:.png=.4bpp.lz)
CLEAN_FILES += $(PNG_FILES:.png=.lz77)
CLEAN_FILES += $(TSA_FILES:.tsa=.tsa.lz)

%.img.bin %.map.bin %.pal.bin: %.png
	@echo "[GEN]	$@"
	@$(GRIT) $< -gB 4 -gzl -m -mLf -mR4 -mzl -pn 16 -ftb -fh! -o $@

CLEAN_FILES += $(PNG_FILES:.png=.img.bin) $(PNG_FILES:.png=.map.bin) $(PNG_FILES:.png=.pal.bin)

# =============
# = PRE-BUILD =
# =============
pre_build: $(PRE_BUILD)

# ==============
# = MAKE CLEAN =
# ==============

clean_basic:
	@rm -f $(CLEAN_FILES)
	@rm -rf $(CLEAN_DIRS)

clean:
	@for i in $(CLEAN_BUILD); do if test -e $$i/makefile ; then $(MAKE) -f $$i/makefile clean || { exit 1;} fi; done;
	@$(MAKE) clean_basic
	@echo "Kernel cleaned .."