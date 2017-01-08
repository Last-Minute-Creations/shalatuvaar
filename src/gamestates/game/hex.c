#include "gamestates/game/hex.h"
#include <ace/types.h>

void hexRectToAxis(tRectCoord *pRect, tAxisCoord *pAxis) {
	pAxis->wX = pRect->wC + ((pRect->wR + 1) >> 1);
	pAxis->wY = pRect->wR;
}

void hexAxisToRect(tAxisCoord *pAxis, tRectCoord *pRect) {
	pRect->wC = pAxis->wX - ((pAxis->wY + 1) >> 1);
	pRect->wR = pAxis->wY;
}

void hexGetAxisFromPixel(UWORD uwX, UWORD uwY, tAxisCoord *pAxis) {
	const UWORD c_uwFirstDx = HEX_WIDTH>>1;
	const UWORD c_uwFirstDy = HEX_HEIGHT>>1;
	
	pAxis->wY = (uwY-c_uwFirstDy + HEX_ADD_Y/2)/HEX_ADD_Y;
	pAxis->wX = ((HEX_WIDTH/2)*pAxis->wY + uwX-c_uwFirstDx + HEX_WIDTH/2)/HEX_WIDTH;
}

/*
// Copypasted from redblob - not sure about correctness of these:

void hexCubeToRect(tCubeCoord *pCube, tRectCoord *pRect) {
	pRect->wC = pCube->wX + ((pCube->wZ - (pCube->wZ & 1)) >> 1);
	pRect->wR = pCube->wZ;
}

void hexRectToCube(tRectCoord *pRect, tCubeCoord *pCube) {
	pCube->wX = pRect->wC - ((pRect->wR - (pRect->wR & 1)) >> 1);
	pCube->wZ = pRect->wR;
	pCube->wY = -pCube->wX -pCube->wZ; // x + y + z = 0
}
*/