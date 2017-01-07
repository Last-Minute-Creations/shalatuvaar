#include "input.h"
#include <clib/exec_protos.h> // Amiga typedefs
#include <clib/intuition_protos.h> // IDCMP_RAWKEY etc
#include "config.h"
#include <ace/config.h>
#include <ace/managers/window.h>
#include <ace/managers/key.h>
#include <ace/managers/mouse.h>
#include <ace/managers/joy.h>

/* Globals */

/* Functions */
void inputOpen() {
	mouseOpen();
	joyOpen();
}

void inputProcess() {
	ULONG ulWindowSignal;
	ULONG ulSignals;
	struct IntuiMessage *pMsg;

	ulWindowSignal = 1L << g_sWindowManager.pWindow->UserPort->mp_SigBit;
	ulSignals = SetSignal(0L, 0L);

	if (ulSignals & ulWindowSignal) {
		while (pMsg = (struct IntuiMessage *) GetMsg(g_sWindowManager.pWindow->UserPort)) {

			switch (pMsg->Class) {
				// Keyboard
				case IDCMP_RAWKEY:
					if (pMsg->Code & IECODE_UP_PREFIX) {
						pMsg->Code -= IECODE_UP_PREFIX;
						keySetState(pMsg->Code, KEY_NACTIVE);
					}
					else if (!keyCheck(pMsg->Code)) {
						keySetState(pMsg->Code, KEY_ACTIVE);
					}
					break;
					
				// Mouse
				case IDCMP_MOUSEBUTTONS:
					if (pMsg->Code & IECODE_UP_PREFIX) {
						pMsg->Code -= IECODE_UP_PREFIX;
						
						mouseSetState(pMsg->Code, MOUSE_NACTIVE);
					}
					else if (!mouseCheck(pMsg->Code)) {
						mouseSetState(pMsg->Code, MOUSE_ACTIVE);
					}
					break;
			}
			ReplyMsg((struct Message *) pMsg);
		}
	}

	joyProcess();
}

void inputClose() {
	mouseClose();
	joyClose();
}