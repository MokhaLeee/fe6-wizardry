#include "prelude.h"
#include "faction.h"
#include "oam.h"

#include "common-chax.h"
#include "UnitSpriteFlip.h"

u8 const gUnitSpritFlipConf[4] = {
    [FACTION_ID_BLUE]   = false,
    [FACTION_ID_GREEN]  = false,
    [FACTION_ID_RED]    = true,
    [FACTION_ID_PURPLE] = true,
};

const u16 Sprite_16x16_HFlipped[] =
{
    1,
    OAM0_SHAPE_16x16, OAM1_SIZE_16x16 + OAM1_HFLIP, 0,
};

const u16 Sprite_16x32_HFlipped[] =
{
    1,
    OAM0_SHAPE_16x32, OAM1_SIZE_16x32 + OAM1_HFLIP, 0,
};

const u16 Sprite_32x32_HFlipped[] =
{
    1,
    OAM0_SHAPE_32x32, OAM1_SIZE_32x32 + OAM1_HFLIP, 0,
};
