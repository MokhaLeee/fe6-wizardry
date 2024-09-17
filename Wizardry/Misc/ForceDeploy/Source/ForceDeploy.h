#pragma once

#include "prelude.h"

struct ForceDeployConf {
    u8 pid;         /* 0 = ANY */
    u8 jid;         /* 0 = ANY */
    u8 chapter;     /* 0xFF = ANY */
    u8 flag;        /* 0 = ANY */
};

extern struct ForceDeployConf const * const gpForceDeployConfTable;
