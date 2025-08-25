#include "prelude.h"
#include "unit.h"
#include "item.h"
#include "common-chax.h"

LYN_REPLACE_CHECK(GetUnitPower);
int GetUnitPower(struct Unit * unit)
{
	// kprintf_test();

	return unit->pow + GetItemPowBonus(GetUnitEquippedWeapon(unit));
}
