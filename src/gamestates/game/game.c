#include "gamestates/game/game.h"
#include <ace/utils/bitmap.h>
#include <ace/utils/bitmapmask.h>
#include <ace/utils/extview.h>
#include <ace/utils/palette.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/key.h>
#include <ace/managers/game.h>
#include <ace/managers/blit.h>

#define HEX_SIZE 64
#define HEX_ADD_X (HEX_SIZE-8)
#define HEX_ADD_Y (HEX_SIZE-14)
#define HEX_COUNT_X 10
#define HEX_COUNT_Y 10

tView *s_pGameView;
tVPort *s_pGameVPort;
tSimpleBufferManager *s_pGameBuffer;

void gsGameCreate(void) {
	tBitMap *pHexGreen;
	tBitmapMask *pHexMask;
	
	s_pGameView = viewCreate(V_GLOBAL_CLUT);
	s_pGameVPort = vPortCreate(s_pGameView, WINDOW_SCREEN_WIDTH, WINDOW_SCREEN_HEIGHT, WINDOW_SCREEN_BPP, 0);
	s_pGameBuffer = simpleBufferCreate(s_pGameVPort, HEX_COUNT_X*HEX_SIZE, HEX_COUNT_Y*HEX_SIZE, BMF_CLEAR);
	paletteLoad("data/amidb32.plt", s_pGameVPort->pPalette, 32);
	pHexGreen = bitmapCreateFromFile("data/hex_grn.bm");
	pHexMask = bitmapMaskCreate("data/hex_mask.msk");
	
	UBYTE x, y;
	UWORD dx, dy;
	dy = 0;
	for(y = 0; y != HEX_COUNT_Y; ++y) {
		if(y & 1)
			dx = ((HEX_ADD_X) >> 1);
		else
			dx = 0;
		for(x = 0; x != HEX_COUNT_X; ++x) {
			blitCopyMask(
				pHexGreen, 0, 0,
				s_pGameBuffer->pBuffer, dx, dy,
				HEX_SIZE, HEX_SIZE,
				pHexMask->pData
			);
			dx += HEX_ADD_X;
		}
		dy += HEX_ADD_Y;
	}
	bitmapDestroy(pHexGreen);
	bitmapMaskDestroy(pHexMask);
	
	viewLoad(s_pGameView);
}

void gsGameLoop(void) {
	if(keyUse(KEY_ESCAPE)) {
		gameClose();
		return;
	}
	WaitTOF();
}

void gsGameDestroy(void) {
	viewLoad(0);
	viewDestroy(s_pGameView);
}