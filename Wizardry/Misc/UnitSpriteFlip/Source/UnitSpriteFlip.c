#include "prelude.h"
#include "bm.h"
#include "faction.h"
#include "unit.h"
#include "oam.h"
#include "sprite.h"
#include "unitsprite.h"
#include "trap.h"
#include "map.h"
#include "armfunc.h"
#include "constants/unitsprites.h"

#include "common-chax.h"
#include "UnitSpriteFlip.h"

LYN_REPLACE_CHECK(RefreshUnitSprites);
void RefreshUnitSprites(void)
{
    struct UnitSprite * map_sprite;

    struct Trap * trap;
    int i;

    gNextFreeUnitSprite = &gUnitSprites[0];

    gNextFreeUnitSprite->next = NULL;
    gNextFreeUnitSprite->y = 0x400;

    gNextFreeUnitSprite = &gUnitSprites[1];

    for (i = 1; i < FACTION_PURPLE + 6; ++i)
    {
        struct Unit * unit = GetUnit(i);

        if (!unit)
            continue;

        if (!unit->pinfo)
            continue;

        unit->map_sprite = NULL;

        if ((unit->flags & (UNIT_FLAG_HIDDEN | UNIT_FLAG_CONCEALED)) != 0)
            continue;

        if (gMapUnit[unit->y][unit->x] == 0)
            continue;

        map_sprite = AddUnitSprite(unit->y * 16);

        map_sprite->y = unit->y * 16;
        map_sprite->x = unit->x * 16;

        map_sprite->oam2 = UseUnitSprite(GetUnitMapSprite(unit)) + OAM2_PAL(GetUnitDisplayedSpritePalette(unit));

        map_sprite->config = GetUnitSpriteInfo(GetUnitMapSprite(unit)).size;

        // map fade units are put on the same hardware layer as the map tiles
        if ((unit->flags & UNIT_FLAG_MAPFADE) != 0)
            map_sprite->config += 3;

#if CHAX
        /* _u0A is unsed, here we use this Byte as an identifier */
        if (gUnitSpritFlipConf[UNIT_FACTION(unit) >> 6])
            map_sprite->_pad_0x0A[0] = true;
        else
            map_sprite->_pad_0x0A[0] = false;
#endif

        unit->map_sprite = map_sprite;
    }

    for (trap = GetTrap(0); trap->kind != TRAP_NONE; ++trap)
    {
        u16 oam2;

        switch (trap->kind)
        {
            case TRAP_BALLISTA:
                oam2 = UseUnitSprite(UNITSPRITE_BALLISTA) - 0x4000;
                break;

            case TRAP_LONGBALLISTA:
                oam2 = UseUnitSprite(UNITSPRITE_LONGBALLISTA) - 0x4000;
                break;

            case TRAP_KILLERBALLISTA:
                oam2 = UseUnitSprite(UNITSPRITE_KILLERBALLISTA) - 0x4000;
                break;

            default:
                continue;
        }

        map_sprite = AddUnitSprite(trap->y * 16);

        map_sprite->y = trap->y * 16;
        map_sprite->x = trap->x * 16;

        map_sprite->oam2 = oam2;

        map_sprite->config = GetUnitSpriteInfo(UNITSPRITE_BALLISTA).size;
    }

    if (gUnitSpriteSyncRequest != 0)
        ForceSyncUnitSpriteSheet();
}

LYN_REPLACE_CHECK(PutUnitSpritesOam);
void PutUnitSpritesOam(void)
{
    struct UnitSprite * it = gUnitSprites->next;

    PutUnitSpriteIconsOam();

    for (; it != NULL; it = it->next)
    {
        int x = it->x - gBmSt.camera.x;
        int y = it->y - gBmSt.camera.y;

        if (x < -16 || x > DISPLAY_WIDTH)
            continue;

        if (y < -32 || y > DISPLAY_HEIGHT)
            continue;

#if CHAX
        if (it->_pad_0x0A[0] == 1)
        {
            switch (it->config) {
            case UNITSPRITE_16x16:
                PutOamHi(OAM1_X(0x200 + x), OAM0_Y(0x100 + y), Sprite_16x16_HFlipped, it->oam2 + OAM2_LAYER(2));
                break;

            case UNITSPRITE_16x32:
                PutOamHi(OAM1_X(0x200 + x), OAM0_Y(0x100 + y - 0x10), Sprite_16x32_HFlipped, it->oam2 + OAM2_LAYER(2));
                break;

            case UNITSPRITE_32x32:
                PutOamHi(OAM1_X(0x200 + x - 8), OAM0_Y(0x100 + y - 0x10), Sprite_32x32_HFlipped, it->oam2 + OAM2_LAYER(2));
                break;

                // map fade units are put on the same hardware layer as the map tiles

            case UNITSPRITE_16x16 + 3:
                PutOamHi(OAM1_X(0x200 + x), OAM0_Y(0x100 + y), Sprite_16x16_HFlipped, it->oam2 + OAM2_LAYER(3));
                break;

            case UNITSPRITE_16x32 + 3:
                PutOamHi(OAM1_X(0x200 + x), OAM0_Y(0x100 + y - 0x10), Sprite_16x32_HFlipped, it->oam2 + OAM2_LAYER(3));
                break;

            case UNITSPRITE_32x32 + 3:
                PutOamHi(OAM1_X(0x200 + x - 8), OAM0_Y(0x100 + y - 0x10), Sprite_32x32_HFlipped, it->oam2 + OAM2_LAYER(3));
                break;
            }
            continue;
        }
#endif

        switch (it->config) {
        case UNITSPRITE_16x16:
            PutOamHi(OAM1_X(0x200 + x), OAM0_Y(0x100 + y), Sprite_16x16, it->oam2 + OAM2_LAYER(2));
            break;

        case UNITSPRITE_16x32:
            PutOamHi(OAM1_X(0x200 + x), OAM0_Y(0x100 + y - 0x10), Sprite_16x32, it->oam2 + OAM2_LAYER(2));
            break;

        case UNITSPRITE_32x32:
            PutOamHi(OAM1_X(0x200 + x - 8), OAM0_Y(0x100 + y - 0x10), Sprite_32x32, it->oam2 + OAM2_LAYER(2));
            break;

            // map fade units are put on the same hardware layer as the map tiles

        case UNITSPRITE_16x16 + 3:
            PutOamHi(OAM1_X(0x200 + x), OAM0_Y(0x100 + y), Sprite_16x16, it->oam2 + OAM2_LAYER(3));
            break;

        case UNITSPRITE_16x32 + 3:
            PutOamHi(OAM1_X(0x200 + x), OAM0_Y(0x100 + y - 0x10), Sprite_16x32, it->oam2 + OAM2_LAYER(3));
            break;

        case UNITSPRITE_32x32 + 3:
            PutOamHi(OAM1_X(0x200 + x - 8), OAM0_Y(0x100 + y - 0x10), Sprite_32x32, it->oam2 + OAM2_LAYER(3));
            break;
        }
    }
}
