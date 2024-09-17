#include "prelude.h"
#include "constants/pids.h"
#include "constants/chapters.h"

#include "common-chax.h"
#include "ForceDeploy.h"

static const struct ForceDeployConf gForceDeployConfTable[] = {
    {
        .pid = PID_ROY,
        .jid = 0,
        .flag = 0,
        .chapter = 0xFF,
    },
    {
        .pid = PID_FAE,
        .jid = 0,
        .flag = 0,
        .chapter = CHAPTER_24,
    },
    {},
};

struct ForceDeployConf const * const gpForceDeployConfTable = gForceDeployConfTable;
FEB_IDENTIFIER(gpForceDeployConfTable);
