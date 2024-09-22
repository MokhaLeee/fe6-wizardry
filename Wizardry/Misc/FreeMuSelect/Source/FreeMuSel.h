#include "prelude.h"
#include "unit.h"
#include "mu.h"

#include "common-chax.h"

struct FreeMuSelSt {
    u32 valid_magic;
    bool moving;
    /* 6C */
};

extern struct FreeMuSelSt gFreeMuSelSt;

#define FREEMUSELST_VALID_MAGIC 0x33224455
#define FreeMuSelValid() (gFreeMuSelSt.valid_magic == FREEMUSELST_VALID_MAGIC)
