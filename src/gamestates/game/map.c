#include "gamestates/game/map.h"
#include <stdio.h>
#include <ace/managers/blit.h>
#include "gamestates/game/game.h"

UBYTE s_pMapData[HEX_COUNT_X][HEX_COUNT_Y];

void mapRectToAxis(tRectCoord *pRect, tAxisCoord *pAxis) {
	pAxis->wX = pRect->wC + ((pRect->wR + 1) >> 1);
	pAxis->wY = pRect->wR;
}

void mapAxisToRect(tAxisCoord *pAxis, tRectCoord *pRect) {
	pRect->wC = pAxis->wX - ((pAxis->wY + 1) >> 1);
	pRect->wR = pAxis->wY;
}

void mapGetPixelAxisCoord(UWORD uwX, UWORD uwY, tAxisCoord *pAxis) {
	const UWORD c_uwFirstDx = HEX_WIDTH>>1;
	const UWORD c_uwFirstDy = HEX_HEIGHT>>1;
	
	pAxis->wY = (uwY-c_uwFirstDy + HEX_ADD_Y/2)/HEX_ADD_Y;
	pAxis->wX = ((HEX_WIDTH/2)*pAxis->wY + uwX-c_uwFirstDx + HEX_WIDTH/2)/HEX_WIDTH;
}

/*
// Copypasted from redblob - not sure about those:

void mapCubeToRect(tCubeCoord *pCube, tRectCoord *pRect) {
	pRect->wC = pCube->wX + ((pCube->wZ - (pCube->wZ & 1)) >> 1);
	pRect->wR = pCube->wZ;
}

void mapRectToCube(tRectCoord *pRect, tCubeCoord *pCube) {
	pCube->wX = pRect->wC - ((pRect->wR - (pRect->wR & 1)) >> 1);
	pCube->wZ = pRect->wR;
	pCube->wY = -pCube->wX -pCube->wZ; // x + y + z = 0
}
*/

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
				mapDrawHex(g_pHexGrn, x, y);
			// }
			// TODO: else - print inactive?
		}
	}
	
	fclose(pMapFile);
}

void mapDrawHex(tBitMap *pHex, UWORD uwX, UWORD uwY) {
	UWORD uwBmX, uwBmY;
	
	uwBmX = uwX*HEX_ADD_X;
	uwBmY = uwY*HEX_ADD_Y;
	if(uwY & 1)
		uwBmX += HEX_ADD_X >> 1;
	blitCopyMask(
		pHex, 0, 0,
		g_pGameBuffer->pBuffer, uwBmX, uwBmY,
		HEX_HEIGHT, HEX_HEIGHT,
		g_pHexMask->pData
	);
}

void mapPlaceBobOnHex(tBob *pBob, UWORD uwX, UWORD uwY) {
	UWORD uwDstX, uwDstY;
	
	uwDstX = (uwX*HEX_WIDTH) + (HEX_WIDTH >> 1) - (pBob->pBitmap->BytesPerRow<<2);
	if(uwY & 1)
		uwDstX += HEX_WIDTH >> 1;
	uwDstY = (uwY)*50 + (HEX_HEIGHT >> 1) - (pBob->uwHeight >> 1); 
	bobDraw(
		pBob, g_pGameBuffer->pBuffer,
		uwDstX, uwDstY
	);
}
