#include "gamestates/game/character.h"
#include <ace/managers/memory.h>
#include "gamestates/game/bob.h"

tCharacter *characterCreate(void) {
	tCharacter *pChar;
	
	pChar = memAllocFast(sizeof(tCharacter));
	pChar->sPos.wX = 4;
	pChar->sPos.wY = 2;
	pChar->pBob = bobUniqueCreate("data/knight.bm", "data/knight.msk", 32, 0);
	
	return pChar;
}

void characterDestroy(tCharacter *pChar) {
	bobUniqueDestroy(pChar->pBob);
	memFree(pChar, sizeof(tCharacter));
}