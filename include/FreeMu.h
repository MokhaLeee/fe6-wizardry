#include "prelude.h"
#include "unit.h"
#include "mu.h"

#include "common-chax.h"

struct FreeMuProc {
    struct MuProc mu;
    i8 ** movemap;
};
