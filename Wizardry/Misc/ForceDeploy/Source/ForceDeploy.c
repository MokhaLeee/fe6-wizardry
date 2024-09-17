#include "prelude.h"
#include "unit.h"
#include "eventinfo.h"
#include "prepscreen.h"

#include "common-chax.h"
#include "ForceDeploy.h"

LYN_REPLACE_CHECK(IsUnitMandatoryDeploy);
bool IsUnitMandatoryDeploy(struct Unit * unit)
{
    const struct ForceDeployConf * it;
    for (it = gpForceDeployConfTable; ; it++)
    {
        if (it->pid == 0 && it->jid == 0)
            break;

        if (it->pid != 0 && UNIT_PID(unit) != it->pid)
            continue;

        if (it->jid != 0 && UNIT_JID(unit) != it->jid)
            continue;

        if (it->chapter != 0xFF && gPlaySt.chapter == it->chapter)
            continue;

        if (it->flag == 0 || CheckFlag(it->flag))
            return true;
    }
    return false;
}
