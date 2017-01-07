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

#define HEX_HEIGHT 64
#define HEX_WIDTH 56 
#define HEX_ADD_X (HEX_HEIGHT-8)
#define HEX_ADD_Y (HEX_HEIGHT-14)
#define HEX_COUNT_X 9
#define HEX_COUNT_Y 15
#define MAP_LINE_BFR_SIZE (HEX_COUNT_X+2+1)

tView *s_pGameView;
tVPort *s_pGameVPort;
tSimpleBufferManager *s_pGameBuffer;
tCameraManager *s_pCamera;
tBob *s_pKnightBob;
tBob *s_pCursor;
UWORD s_uwMouseMapX, s_uwMouseMapY;

UWORD s_uwSelX, s_uwSelY, s_uwPrevSelX, s_uwPrevSelY;

tBitmapMask *s_pHexMask;
tBitMap *s_pHexSel;
tBitMap *s_pHexGrn;

/*
 *	Map is stored in rectangular coord system.
 *	For sake of simplifying calculations, they are done in cube system.
 *	All coords are stored in cube system as well. 
 */
 
typedef struct _tCubeCoord {
	WORD wX;
	WORD wY;
	WORD wZ;
} tCubeCoord;

typedef struct _tRectCoord {
	WORD wC; ///< Column
	WORD wR; ///< Row
} tRectCoord;

void cubeToRect(tCubeCoord *pCube, tRectCoord *pRect) {
	pRect->wC = pCube->wX + ((pCube->wZ - (pCube->wZ & 1)) >> 1);
	pRect->wR = pCube->wZ;
}

void rectToCube(tRectCoord *pRect, tCubeCoord *pCube) {
	pCube->wX = pRect->wC - ((pRect->wR - (pRect->wR & 1)) >> 1);
	pCube->wZ = pRect->wR;
	pCube->wY = -pCube->wX -pCube->wZ; // x + y + z = 0
}

UBYTE s_pMapData[HEX_COUNT_X][HEX_COUNT_Y];

void mapPlaceBobOnHex(tBob *pBob, UWORD uwX, UWORD uwY) {
	UWORD uwDstX, uwDstY;
	
	uwDstX = (uwX*HEX_WIDTH) + (HEX_WIDTH >> 1) - (pBob->pBitmap->BytesPerRow<<2);
	if(uwY & 1)
		uwDstX += HEX_WIDTH >> 1;
	uwDstY = (uwY)*50 + (HEX_HEIGHT >> 1) - (pBob->uwHeight >> 1); 
	bobDraw(
		pBob, s_pGameBuffer->pBuffer,
		uwDstX, uwDstY
	);
}

void mapDrawHex(tBitMap *pHex, UWORD uwX, UWORD uwY) {
	UWORD uwBmX, uwBmY;
	
	uwBmX = uwX*HEX_ADD_X;
	uwBmY = uwY*HEX_ADD_Y;
	if(uwY & 1)
		uwBmX += HEX_ADD_X >> 1;
	blitCopyMask(
		pHex, 0, 0,
		s_pGameBuffer->pBuffer, uwBmX, uwBmY,
		HEX_HEIGHT, HEX_HEIGHT,
		s_pHexMask->pData
	);
}

void mapCreate(void) {
	UBYTE x, y;
	FILE *pMapFile;
	char szBfr[MAP_LINE_BFR_SIZE];
	
	// Open file for reading
	pMapFile = fopen("data/map.txt", "rb");
		
	// Read from file & draw
	for(y = 0; y != HEX_COUNT_Y; ++y) {
		fgets(szBfr, MAP_LINE_BFR_SIZE, pMapFile);
		for(x = 0; x != HEX_COUNT_X; ++x) {
			s_pMapData[x][y] = szBfr[x];
			// if(szBfr[x] == '#') {
				mapDrawHex(s_pHexGrn, x, y);
			// }
			// TODO: else - print inactive?
		}
	}
	
	fclose(pMapFile);
}

UWORD g_uwBobX, g_uwBobY;

void gsGameCreate(void) {
	s_uwSelX = 0; s_uwPrevSelX = 0;
	s_uwSelY = 0; s_uwPrevSelY = 0;
	s_pGameView = viewCreate(V_GLOBAL_CLUT);
	s_pGameVPort = vPortCreate(s_pGameView, WINDOW_SCREEN_WIDTH, WINDOW_SCREEN_HEIGHT, WINDOW_SCREEN_BPP, 0);
	s_pGameBuffer = simpleBufferCreate(s_pGameVPort, HEX_COUNT_X*HEX_HEIGHT, HEX_HEIGHT+(HEX_COUNT_Y-1)*50, BMF_CLEAR);
	paletteLoad("data/amidb32.plt", s_pGameVPort->pPalette, 32);
	s_pCamera = s_pGameBuffer->pCameraManager;
	
	s_pHexMask = bitmapMaskCreate("data/hex_mask.msk");
	s_pHexSel = bitmapCreateFromFile("data/hex_sel.bm");
	s_pHexGrn = bitmapCreateFromFile("data/hex_grn.bm");

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
	if(ubCursorOnNewTile) {
		if(ubCursorOnNewTile == 1) {
			// TODO: Undraw all objects on old & new tile
			bobUndraw(s_pKnightBob, s_pGameBuffer->pBuffer);
			// Undraw selection
			mapDrawHex(s_pHexGrn, s_uwPrevSelX, s_uwPrevSelY);
		}
		// Draw selection on new tile
		mapDrawHex(s_pHexSel, s_uwSelX, s_uwSelY);
		// TODO: Draw all objects on old & new tile
		mapPlaceBobOnHex(s_pKnightBob, g_uwBobX, g_uwBobY);
		ubCursorOnNewTile = 0;
	}
	s_uwPrevSelX = s_uwSelX;
	s_uwPrevSelY = s_uwSelY;
	
	if(keyUse(KEY_ESCAPE)) {
		gameClose();
		return;
	}
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
	
	s_uwMouseMapX = mouseGetX() + s_pCamera->uPos.sUwCoord.uwX;
	s_uwMouseMapY = mouseGetY() + s_pCamera->uPos.sUwCoord.uwY;
	
	if(keyUse(KEY_W)) {
		// if(g_uwBobY && s_pMapData[g_uwBobX][g_uwBobY-1] == '#') {
			// --g_uwBobY;
			// bobUndraw(s_pKnightBob, s_pGameBuffer->pBuffer);
			// mapPlaceBobOnHex(s_pKnightBob, g_uwBobX, g_uwBobY);
		// }
		--s_uwSelY;
		ubCursorOnNewTile = 1;
	}
	if(keyUse(KEY_S)) {
		// if(g_uwBobY < HEX_COUNT_Y-1 && s_pMapData[g_uwBobX][g_uwBobY+1] == '#') {
			// ++g_uwBobY;
			// bobUndraw(s_pKnightBob, s_pGameBuffer->pBuffer);
			// mapPlaceBobOnHex(s_pKnightBob, g_uwBobX, g_uwBobY);
		// }
		++s_uwSelY;
		ubCursorOnNewTile = 1;
	}
	if(keyUse(KEY_A)) {
		// if(g_uwBobX && s_pMapData[g_uwBobX-1][g_uwBobY] == '#') {
			// --g_uwBobX;
			// bobUndraw(s_pKnightBob, s_pGameBuffer->pBuffer);
			// mapPlaceBobOnHex(s_pKnightBob, g_uwBobX, g_uwBobY);
		// }
		--s_uwSelX;
		ubCursorOnNewTile = 1;
	}
	if(keyUse(KEY_D)) {
		// if(g_uwBobX < HEX_COUNT_X-1 && s_pMapData[g_uwBobX+1][g_uwBobY] == '#') {
			// ++g_uwBobX;
			// bobUndraw(s_pKnightBob, s_pGameBuffer->pBuffer);
			// mapPlaceBobOnHex(s_pKnightBob, g_uwBobX, g_uwBobY);
		// }
		++s_uwSelX;
		ubCursorOnNewTile = 1;
	}
	
	if(mouseUse(MOUSE_LMB)) {
		logWrite("Mouse pos: %u, %u\n", mouseGetX(), mouseGetY());
	}
	
	bobDraw(s_pCursor, s_pGameBuffer->pBuffer, s_uwMouseMapX, s_uwMouseMapY);

	viewProcessManagers(s_pGameView);
	copProcessBlocks();
	vPortWaitForEnd(s_pGameVPort);
	
	bobUndraw(s_pCursor, s_pGameBuffer->pBuffer);
}

void gsGameDestroy(void) {
	viewLoad(0);
	
	bitmapMaskDestroy(s_pHexMask);
	bitmapDestroy(s_pHexSel);
	bitmapDestroy(s_pHexGrn);
	
	viewDestroy(s_pGameView);
	bobUniqueDestroy(s_pKnightBob);
	bobUniqueDestroy(s_pCursor);
}