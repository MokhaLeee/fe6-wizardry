#include "prelude.h"
#include "hardware.h"
#include "m4a.h"
#include "move.h"
#include "debugtext.h"

static void on_vblank(void)
{
	INTR_CHECK = INTR_FLAG_VBLANK;

	SyncDispIo();
	SyncBgsAndPal();
	ApplyDataMoves();

	m4aSoundVSync();
	m4aSoundMain();
}

void StartDebugScreen(void)
{
	SetOnVBlank(on_vblank);

	DebugInitBg(BG_2, 0);
	SetBgOffset(BG_2, 0, 0);

	SetDispEnable(0, 0, 1, 0, 0);
	SetWinEnable(0, 0, 0);
	SetBlendNone();

	/* Idle */
	while (1) {
		RefreshKeySt(gKeySt);

		DebugUpdateScreen(gKeySt->held, gKeySt->pressed);

		VBlankIntrWait();
		SoftResetIfKeyCombo();
	}
}

/* Example */
void DebugScreenExample(void)
{
	DebugPrintStr("helloworld, this is mokha\n");
	StartDebugScreen();
}
