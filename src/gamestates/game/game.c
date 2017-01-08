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

tView *s_pGameView;
tVPort *s_pGameVPort;
tCameraManager *s_pCamera;
tSimpleBufferManager *g_pGameBuffer;

tBob *s_pKnightBob;
tBob *s_pCursor;

UWORD s_uwMouseMapX, s_uwMouseMapY;
UWORD s_uwSelX, s_uwSelY, s_uwPrevSelX, s_uwPrevSelY;

tBitMap *s_pHexSel;
tBitmapMask *g_pHexMask;
tBitMap *g_pHexGrn;

UWORD g_uwBobX, g_uwBobY;

void gsGameCreate(void) {
	s_uwSelX = 0; s_uwPrevSelX = 0;
	s_uwSelY = 0; s_uwPrevSelY = 0;
	s_pGameView = viewCreate(V_GLOBAL_CLUT);
	s_pGameVPort = vPortCreate(s_pGameView, WINDOW_SCREEN_WIDTH, WINDOW_SCREEN_HEIGHT, WINDOW_SCREEN_BPP, 0);
	g_pGameBuffer = simpleBufferCreate(s_pGameVPort, HEX_COUNT_X*HEX_HEIGHT, HEX_HEIGHT+(HEX_COUNT_Y-1)*50, BMF_CLEAR);
	paletteLoad("data/amidb32.plt", s_pGameVPort->pPalette, 32);
	s_pCamera = g_pGameBuffer->pCameraManager;
	
	g_pHexMask = bitmapMaskCreate("data/hex_mask.msk");
	s_pHexSel = bitmapCreateFromFile("data/hex_sel.bm");
	g_pHexGrn = bitmapCreateFromFile("data/hex_grn.bm");

	mapCreate();
	
	s_pKnightBob = bobUniqueCreate("data/knight.bm", "data/knight.msk", 32, 0);
	s_pCursor = bobUniqueCreate("data/cursor.bm", "data/cursor.msk", 16, 0);
	g_uwBobX = 3;
	g_uwBobY = 2;
	mapPlaceBobOnHex(s_pKnightBob, g_uwBobX, g_uwBobY);
	viewLoad(s_pGameView);
}

void gsGameLoop(void) {
	static UBYTE ubCursorOnNewTile = 2;
	
	if(keyUse(KEY_ESCAPE)) {
		gameClose();
		return;
	}
	
	if(ubCursorOnNewTile) {
		if(ubCursorOnNewTile == 1) {
			// TODO: Undraw all objects on old & new tile
			bobUndraw(s_pKnightBob, g_pGameBuffer->pBuffer);
			// Undraw selection
			mapDrawHex(g_pHexGrn, s_uwPrevSelX, s_uwPrevSelY);
		}
		// Draw selection on new tile
		mapDrawHex(s_pHexSel, s_uwSelX, s_uwSelY);
		// TODO: Draw all objects on old & new tile
		mapPlaceBobOnHex(s_pKnightBob, g_uwBobX, g_uwBobY);
		ubCursorOnNewTile = 0;
	}
	s_uwPrevSelX = s_uwSelX;
	s_uwPrevSelY = s_uwSelY;
	
	// Scroll screen
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
	s_uwMouseMapX = mouseGetX() + s_pCamera->uPos.sUwCoord.uwX;
	s_uwMouseMapY = mouseGetY() + s_pCamera->uPos.sUwCoord.uwY;
	tAxisCoord sMouseAxisCoord;
	tRectCoord sMouseRectCoord;
	mapGetPixelAxisCoord(s_uwMouseMapX, s_uwMouseMapY, &sMouseAxisCoord);
	mapAxisToRect(&sMouseAxisCoord, &sMouseRectCoord);
	if(
		sMouseRectCoord.wC >= 0 && sMouseRectCoord.wC < HEX_COUNT_X &&
		sMouseRectCoord.wR >= 0 && sMouseRectCoord.wR < HEX_COUNT_Y
	) {
		s_uwSelY = sMouseRectCoord.wR;
		s_uwSelX = sMouseRectCoord.wC;
		if(s_uwSelX != s_uwPrevSelX || s_uwSelY != s_uwPrevSelY)
			ubCursorOnNewTile = 1;
	}
	
	// DEBUG
	if(mouseUse(MOUSE_LMB)) {
		logWrite(
			"Mouse pos on bitmap: %u, %u, axis: %d, %d, rect: %d, %d\n",
			s_uwMouseMapX, s_uwMouseMapY,
			sMouseAxisCoord.wX, sMouseAxisCoord.wY,
			sMouseRectCoord.wC, sMouseRectCoord.wR
		);
	}
	
	bobDraw(s_pCursor, g_pGameBuffer->pBuffer, s_uwMouseMapX, s_uwMouseMapY);

	viewProcessManagers(s_pGameView);
	copProcessBlocks();
	vPortWaitForEnd(s_pGameVPort);
	
	bobUndraw(s_pCursor, g_pGameBuffer->pBuffer);
}

void gsGameDestroy(void) {
	viewLoad(0);
	
	bitmapMaskDestroy(g_pHexMask);
	bitmapDestroy(s_pHexSel);
	bitmapDestroy(g_pHexGrn);
	
	viewDestroy(s_pGameView);
	bobUniqueDestroy(s_pKnightBob);
	bobUniqueDestroy(s_pCursor);
}