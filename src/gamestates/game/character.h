#ifndef GUARD_MELEE_GAMESTATES_GAME_CHARACTER_H
#define GUARD_MELEE_GAMESTATES_GAME_CHARACTER_H

#include <ace/types.h>
#include "gamestates/game/bob.h"
#include "gamestates/game/hex.h"

typedef struct _tCharacter {
	tAxisCoord sPos;
	tBob *pBob;
} tCharacter;

tCharacter *characterCreate(void);

void characterDestroy(tCharacter *pCharacter);

#endif // GUARD_MELEE_GAMESTATES_GAME_CHARACTER_H
