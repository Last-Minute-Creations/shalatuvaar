#include "gamestates/game/game.h"
#include <stdio.h>
#include <ace/utils/bitmap.h>
#include <ace/utils/bitmapmask.h>
#include <ace/utils/extview.h>
#include <ace/utils/palette.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/key.h>
#include <ace/managers/mouse.h>
#include <ace/managers/game.h>
#include <ace/managers/blit.h>
#include "gamestates/game/bob.h"
#include "gamestates/game/map.h"
#include "gamestates/game/character.h"

tView *s_pGameView;
tVPort *s_pGameVPort;
tCameraManager *s_pCamera;
tSimpleBufferManager *g_pGameBuffer;
tCharacter *s_pMainChar;

tBob *s_pCursor;

tAxisCoord s_sMouseAxisCoord, s_sMousePrevAxisCoord;

tBitMap *s_pHexSel;
tBitmapMask *g_pHexMask;
tBitMap *g_pHexGrn;

void gsGameCreate(void) {
	s_sMouseAxisCoord.wX = 0; s_sMouseAxisCoord.wY = 0;
	s_sMousePrevAxisCoord.wX = 0; s_sMousePrevAxisCoord.wY = 0;
	s_pGameView = viewCreate(V_GLOBAL_CLUT);
	s_pGameVPort = vPortCreate(s_pGameView, WINDOW_SCREEN_WIDTH, WINDOW_SCREEN_HEIGHT, WINDOW_SCREEN_BPP, 0);
	g_pGameBuffer = simpleBufferCreate(s_pGameVPort, HEX_COUNT_X*HEX_HEIGHT, HEX_HEIGHT+(HEX_COUNT_Y-1)*50, BMF_CLEAR);
	paletteLoad("data/amidb32.plt", s_pGameVPort->pPalette, 32);
	s_pCamera = g_pGameBuffer->pCameraManager;
	
	g_pHexMask = bitmapMaskCreate("data/hex_mask.msk");
	s_pHexSel = bitmapCreateFromFile("data/hex_sel.bm");
	g_pHexGrn = bitmapCreateFromFile("data/hex_grn.bm");

	mapCreate();
	s_pCursor = bobUniqueCreate("data/cursor.bm", "data/cursor.msk", 16, 0);
	
	s_pMainChar = characterCreate();
	mapPlaceCharacterOnHex(s_pMainChar);
	viewLoad(s_pGameView);
}

void gsGameLoop(void) {
	static UBYTE ubCursorOnNewTile = 2;
	
	if(keyUse(KEY_ESCAPE)) {
		gameClose();
		return;
	}
	
	// Do a scroll if mouse is on edge of screen
	if(mouseGetX() >= 320 - 6) {
		cameraMove(s_pCamera, 2, 0);
	}
	if(mouseGetX() <= 0+6) {
		cameraMove(s_pCamera, -2, 0);
	}
	if(mouseGetY() <= 0+6) {
		cameraMove(s_pCamera, 0, -2);
	}
	if(mouseGetY() >= 256 - 6) {
		cameraMove(s_pCamera, 0, 2);
	}
	
	// Detect hex under cursor
	UWORD uwMouseMapX, uwMouseMapY;
	uwMouseMapX = mouseGetX() + s_pCamera->uPos.sUwCoord.uwX;
	uwMouseMapY = mouseGetY() + s_pCamera->uPos.sUwCoord.uwY;
	hexGetAxisFromPixel(uwMouseMapX, uwMouseMapY, &s_sMouseAxisCoord);
	
	// Check tile cursor position with map constraints
	mapCheckAxisConstraints(&s_sMouseAxisCoord);
	
	if(
		s_sMouseAxisCoord.wX != s_sMousePrevAxisCoord.wX ||
		s_sMouseAxisCoord.wY != s_sMousePrevAxisCoord.wY
	)
		++ubCursorOnNewTile;
	
	if(ubCursorOnNewTile) {
		if(ubCursorOnNewTile == 1) {
			// TODO: Undraw all objects on old & new tile - not needed later
			bobUndraw(s_pMainChar->pBob, g_pGameBuffer->pBuffer);
			// Undraw selection
			mapDrawAxisHex(g_pHexGrn, &s_sMousePrevAxisCoord);
		}
		// Draw selection on new tile
		mapDrawAxisHex(s_pHexSel, &s_sMouseAxisCoord);
		// TODO: Draw all objects on old & new tile.
		// remove undraw if drawing only needed below.
		mapPlaceCharacterOnHex(s_pMainChar);
		ubCursorOnNewTile = 0;
	}
	
	// DEBUG
	// if(mouseUse(MOUSE_LMB)) {
		// logWrite(
			// "Mouse pos on bitmap: %u, %u, axis: %d, %d, rect: %d, %d\n",
			// uwMouseMapX, uwMouseMapY,
			// sMouseAxisCoord.wX, sMouseAxisCoord.wY,
			// sMouseRectCoord.wC, sMouseRectCoord.wR
		// );
	// }
	if(mouseUse(MOUSE_LMB)) {
		s_pMainChar->sPos.wX = s_sMouseAxisCoord.wX;
		s_pMainChar->sPos.wY = s_sMouseAxisCoord.wY;
		bobUndraw(s_pMainChar->pBob, g_pGameBuffer->pBuffer);
		mapPlaceCharacterOnHex(s_pMainChar);
	}
	
	bobDraw(s_pCursor, g_pGameBuffer->pBuffer, uwMouseMapX, uwMouseMapY);

	viewProcessManagers(s_pGameView);
	copProcessBlocks();
	vPortWaitForEnd(s_pGameVPort);
	
	bobUndraw(s_pCursor, g_pGameBuffer->pBuffer);
	
	// Update "previous" mouse coord on map
	s_sMousePrevAxisCoord.wX = s_sMouseAxisCoord.wX;
	s_sMousePrevAxisCoord.wY = s_sMouseAxisCoord.wY;
}

void gsGameDestroy(void) {
	viewLoad(0);
	
	characterDestroy(s_pMainChar);
	bitmapMaskDestroy(g_pHexMask);
	bitmapDestroy(s_pHexSel);
	bitmapDestroy(g_pHexGrn);
	
	viewDestroy(s_pGameView);
	bobUniqueDestroy(s_pCursor);
}