#include "prelude.h"
#include "proc.h"
#include "sound.h"
#include "hardware.h"
#include "movepath.h"
#include "mapwork.h"
#include "playerphase.h"
#include "constants/songs.h"

#include "common-chax.h"
#include "FreeMuSel.h"

LYN_REPLACE_CHECK(InitMovePath);
void InitMovePath(bool display_only)
{
    gFreeMuSelSt.valid_magic = FREEMUSELST_VALID_MAGIC;
    gFreeMuSelSt.moving = false;
}

LYN_REPLACE_CHECK(PlayerPhase_BeginMove);
void PlayerPhase_BeginMove(ProcPtr proc)
{
    gFreeMuSelSt.valid_magic = 0;
}

static bool GetMuPosition(struct Vec2i * out)
{
    struct MuProc * mu = FindProc(ProcScr_Mu);
    if (!mu)
        return false;

    out->x = ((mu->x_q4 + mu->x_offset_q4) >> 4);
    out->y = ((mu->y_q4 + mu->y_offset_q4) >> 4);
    return true;
}

LYN_REPLACE_CHECK(HandlePlayerMapCursor);
void HandlePlayerMapCursor(void)
{
    struct MuProc * mu = FindProc(ProcScr_Mu);

    if (mu && mu->state == MU_STATE_MOVEMENT)
    {
        gKeySt->pressed &= ~(KEY_BUTTON_A | KEY_BUTTON_B | KEY_BUTTON_START | KEY_BUTTON_R | KEY_BUTTON_L);
        return;
    }

    if ((gKeySt->held & KEY_BUTTON_B) && !(gBmSt.cursor_sprite.x & 7) && !(gBmSt.cursor_sprite.y & 7))
    {
        HandleMapCursorInput(gKeySt->pressed2);

        HandleMoveMapCursor(8);
        HandleMoveCameraWithMapCursor(8);
    }
    else
    {
        HandleMapCursorInput(gKeySt->repeated);

        HandleMoveMapCursor(4);
        HandleMoveCameraWithMapCursor(4);
    }

    if ((gBmSt.cursor_sprite.x | gBmSt.cursor_sprite.y) & 0xF)
        gKeySt->pressed &= ~(KEY_BUTTON_A | KEY_BUTTON_B | KEY_BUTTON_START | KEY_BUTTON_R | KEY_BUTTON_L);
    else if (mu && mu->state != MU_STATE_INACTIVE)
    {
        struct Vec2i pos = {0, 0};
        if (!GetMuPosition(&pos))
            return;

        if ((pos.x != gBmSt.cursor.x) || (pos.y != gBmSt.cursor.y))
        {
            BuildBestMoveScript(gBmSt.cursor.x, gBmSt.cursor.y, gWorkingMoveScr);
            SetAutoMuMoveScript(gWorkingMoveScr);
        }
    }
}
