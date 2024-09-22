#include "proc.h"
#include "hardware.h"
#include "util.h"

#include "common-chax.h"

struct ProcEarlyUI {
    PROC_HEADER;

    int timer;
};

static void InitNintendoGfx(struct ProcEarlyUI * proc)
{
    return;
}

const struct ProcScr ProcScr_GameIntroEarlyUI[] = {
    PROC_SLEEP(0),
    PROC_CALL(InitNintendoGfx),
    PROC_END
};
