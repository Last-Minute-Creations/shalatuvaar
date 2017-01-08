#ifndef GUARD_MELLE_GAMESTATES_GAME_MAP_H
#define GUARD_MELLE_GAMESTATES_GAME_MAP_H

#include <ace/config.h>
#include <ace/utils/bitmap.h>
#include "gamestates/game/bob.h"

#define HEX_COUNT_X 9
#define HEX_COUNT_Y 15
#define HEX_HEIGHT 64
#define HEX_WIDTH 56 
#define HEX_ADD_X (HEX_HEIGHT-8)
#define HEX_ADD_Y (HEX_HEIGHT-14)
#define MAP_LINE_BFR_SIZE (HEX_COUNT_X+2+1)

/*
 *	Map is stored in rectangular coord system.
 *	For sake of simplifying calculations, they are done in axis system.
 *	All coords are stored in axis system as well. 
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

typedef struct _tAxisCoord {
	WORD wX; ///< going right
	WORD wY; ///< going left-bottom
} tAxisCoord;

void mapRectToAxis(tRectCoord *pRect, tAxisCoord *pAxis);
void mapAxisToRect(tAxisCoord *pAxis, tRectCoord *pRect);
void mapGetPixelAxisCoord(UWORD uwX, UWORD uwY, tAxisCoord *pAxis);
void mapCreate(void);
void mapDrawHex(tBitMap *pHex, UWORD uwX, UWORD uwY);
void mapPlaceBobOnHex(tBob *pBob, UWORD uwX, UWORD uwY);

#endif // GUARD_MELLE_GAMESTATES_GAME_MAP_H
