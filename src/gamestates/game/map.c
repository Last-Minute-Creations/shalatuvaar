#include "gamestates/game/map.h"
#include <stdio.h>
#include <ace/managers/blit.h>
#include "gamestates/game/game.h"
#include "gamestates/game/hex.h"

UBYTE s_pMapData[HEX_COUNT_X][HEX_COUNT_Y];

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

void mapPlaceCharacterOnHex(tCharacter *pChar) {
	UWORD uwDstX, uwDstY;
	tRectCoord sRect;
	
	hexAxisToRect(&pChar->sPos, &sRect);
	
	uwDstX = (sRect.wC*HEX_WIDTH) + (HEX_WIDTH >> 1) - (pChar->pBob->pBitmap->BytesPerRow<<2);
	if(sRect.wR & 1)
		uwDstX += HEX_WIDTH >> 1;
	uwDstY = (sRect.wR)*50 + (HEX_HEIGHT >> 1) - (pChar->pBob->uwHeight >> 1); 
	bobDraw(
		pChar->pBob, g_pGameBuffer->pBuffer,
		uwDstX, uwDstY
	);
}
