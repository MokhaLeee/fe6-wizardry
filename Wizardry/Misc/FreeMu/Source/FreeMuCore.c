#include "prelude.h"
#include "unit.h"
#include "proc.h"
#include "mu.h"
#include "hardware.h"

#include "common-chax.h"
#include "FreeMu.h"

static void FreeMu_End(struct FreeMuProc * proc);
static void FreeMu_Loop(struct FreeMuProc * proc);

const struct ProcScr ProcScr_FreeMu[] = {
    PROC_NAME("FreeMu"),
    PROC_MARK(PROC_MARK_MU),
    PROC_ONEND(FreeMu_End),

    PROC_YIELD,
    PROC_REPEAT(FreeMu_Loop),

    PROC_END
};

void StartFreeMu(struct Unit * unit, u8 ** movemap)
{
    struct FreeMuProc * proc = (ProcPtr)StartMu(unit);
    Proc_GotoScript(proc, ProcScr_FreeMu); // lol

    proc->movemap = (i8 **)movemap;
}

static void FreeMu_End(struct FreeMuProc * proc)
{
    proc->mu.config->id = 0;
    EndSpriteAnim(proc->mu.sprite_anim);
}

static void FreeMu_Core(struct FreeMuProc * proc)
{
}

static void FreeMu_Loop(struct FreeMuProc * proc)
{
    struct MuProc * mu = &proc->mu;

    if (mu->state != MU_STATE_NONE)
    {
        if (mu->move_clock_q4 == 0)
        {
            if (mu->state == MU_STATE_SLEEPING || mu->state == MU_STATE_MOVEMENT)
                RunMuMoveScript(mu);
        }

        sMuStateFuncs[mu->state](mu);
    }

    PutMu(mu);
}
