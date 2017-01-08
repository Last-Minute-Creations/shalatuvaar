#ifndef GUARD_MELEE_GAMESTATES_GAME_HEX_H
#define GUARD_MELEE_GAMESTATES_GAME_HEX_H

#include <ace/types.h>

/// Hex geometry
#define HEX_COUNT_X 9
#define HEX_COUNT_Y 15
#define HEX_HEIGHT 64
#define HEX_WIDTH 56 
#define HEX_ADD_X (HEX_HEIGHT-8)
#define HEX_ADD_Y (HEX_HEIGHT-14)


/*
 *	Map is stored in rectangular coord system.
 *	For the sake of simplifying calculations, they are done in axis system.
 *	All coords are stored in axis system as well. 
 */
 
/*
// Prob'ly not needed
typedef struct _tCubeCoord {
	WORD wX;
	WORD wY;
	WORD wZ;
} tCubeCoord;
*/

typedef struct _tRectCoord {
	WORD wC; ///< Column
	WORD wR; ///< Row
} tRectCoord;

typedef struct _tAxisCoord {
	WORD wX; ///< going right
	WORD wY; ///< going left-bottom
} tAxisCoord;

void hexRectToAxis(tRectCoord *pRect, tAxisCoord *pAxis);
void hexAxisToRect(tAxisCoord *pAxis, tRectCoord *pRect);
void hexGetAxisFromPixel(UWORD uwX, UWORD uwY, tAxisCoord *pAxis);

#endif // GUARD_MELEE_GAMESTATES_GAME_HEX_H
