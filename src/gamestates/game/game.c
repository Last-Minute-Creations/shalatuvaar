#include "gamestates/game/game.h"
#include <stdio.h>
#include <ace/utils/bitmap.h>
#include <ace/utils/bitmapmask.h>
#include <ace/utils/extview.h>
#include <ace/utils/palette.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/key.h>
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

UBYTE s_pMapData[HEX_COUNT_X][HEX_COUNT_Y];

void mapPlaceBob(tBob *pBob, UWORD uwX, UWORD uwY) { // 3, 2
	UWORD uwDstX, uwDstY;
	
	uwDstX = (uwX*HEX_WIDTH) + (HEX_WIDTH >> 1) - (s_pKnightBob->pBitmap->BytesPerRow<<2);
	if(uwY & 1)
		uwDstX += HEX_WIDTH >> 1;
	uwDstY = (uwY)*50 + (HEX_HEIGHT/2) - (s_pKnightBob->uwHeight >> 1); 
	bobDraw(
		pBob, s_pGameBuffer->pBuffer,
		uwDstX, uwDstY
	);
}

void mapCreate(void) {
	UBYTE x, y;
	UWORD dx, dy;
	tBitMap *pHexGreen;
	tBitmapMask *pHexMask;
	FILE *pMapFile;
	char szBfr[MAP_LINE_BFR_SIZE];
	
	// Open file for reading
	pMapFile = fopen("data/map.txt", "rb");
	
	// Prepare Gfx
	pHexGreen = bitmapCreateFromFile("data/hex_grn.bm");
	pHexMask = bitmapMaskCreate("data/hex_mask.msk");
	
	// Read from file & draw
	dy = 0;
	for(y = 0; y != HEX_COUNT_Y; ++y) {
		fgets(szBfr, MAP_LINE_BFR_SIZE, pMapFile);
		if(y & 1)
			dx = ((HEX_ADD_X) >> 1);
		else
			dx = 0;
		for(x = 0; x != HEX_COUNT_X; ++x) {
			s_pMapData[x][y] = szBfr[x];
			if(szBfr[x] == '#') {
				blitCopyMask(
					pHexGreen, 0, 0,
					s_pGameBuffer->pBuffer, dx, dy,
					HEX_HEIGHT, HEX_HEIGHT,
					pHexMask->pData
				);
			}
			// TODO: else - print inactive?
			dx += HEX_ADD_X;
		}
		dy += HEX_ADD_Y;
	}
	
	fclose(pMapFile);
	bitmapDestroy(pHexGreen);
	bitmapMaskDestroy(pHexMask);
}

void mapMoveBob(tBob *pBob, UWORD uwHexX, UWORD uwHexY) {
	bobUndraw(s_pKnightBob, s_pGameBuffer->pBuffer);
	mapPlaceBob(pBob, uwHexX, uwHexY);
}

UWORD g_uwBobX, g_uwBobY;

void gsGameCreate(void) {
	s_pGameView = viewCreate(V_GLOBAL_CLUT);
	s_pGameVPort = vPortCreate(s_pGameView, WINDOW_SCREEN_WIDTH, WINDOW_SCREEN_HEIGHT, WINDOW_SCREEN_BPP, 0);
	s_pGameBuffer = simpleBufferCreate(s_pGameVPort, HEX_COUNT_X*HEX_HEIGHT, HEX_HEIGHT+(HEX_COUNT_Y-1)*50, BMF_CLEAR);
	paletteLoad("data/amidb32.plt", s_pGameVPort->pPalette, 32);
	s_pCamera = s_pGameBuffer->pCameraManager;

	mapCreate();
	
	s_pKnightBob = bobUniqueCreate("data/knight.bm", "data/knight.msk", 32, 0);
	g_uwBobX = 3;
	g_uwBobY = 2;
	mapPlaceBob(s_pKnightBob, g_uwBobX, g_uwBobY);
	viewLoad(s_pGameView);
}

void gsGameLoop(void) {
	
	
	if(keyUse(KEY_ESCAPE)) {
		gameClose();
		return;
	}
	if(keyCheck(KEY_RIGHT)) {
		cameraMove(s_pCamera, 2, 0);
	}
	if(keyCheck(KEY_LEFT)) {
		cameraMove(s_pCamera, -2, 0);
	}
	if(keyCheck(KEY_UP)) {
		cameraMove(s_pCamera, 0, -2);
	}
	if(keyCheck(KEY_DOWN)) {
		cameraMove(s_pCamera, 0, 2);
	}
	
	if(keyUse(KEY_W)) {
		if(g_uwBobY && s_pMapData[g_uwBobX][g_uwBobY-1] == '#') {
			--g_uwBobY;
			mapMoveBob(s_pKnightBob, g_uwBobX, g_uwBobY);
		}
	}
	if(keyUse(KEY_S)) {
		if(g_uwBobY < HEX_COUNT_Y-1 && s_pMapData[g_uwBobX][g_uwBobY+1] == '#') {
			++g_uwBobY;
			mapMoveBob(s_pKnightBob, g_uwBobX, g_uwBobY);
		}
	}
	if(keyUse(KEY_A)) {
		if(g_uwBobX && s_pMapData[g_uwBobX-1][g_uwBobY] == '#') {
			--g_uwBobX;
			mapMoveBob(s_pKnightBob, g_uwBobX, g_uwBobY);
		}
	}
	if(keyUse(KEY_D)) {
		if(g_uwBobX < HEX_COUNT_X-1 && s_pMapData[g_uwBobX+1][g_uwBobY] == '#') {
			++g_uwBobX;
			mapMoveBob(s_pKnightBob, g_uwBobX, g_uwBobY);
		}
	}

	viewProcessManagers(s_pGameView);
	copProcessBlocks();
	WaitTOF();
}

void gsGameDestroy(void) {
	viewLoad(0);
	viewDestroy(s_pGameView);
	bobUniqueDestroy(s_pKnightBob);
}