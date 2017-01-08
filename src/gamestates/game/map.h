#ifndef GUARD_MELLE_GAMESTATES_GAME_MAP_H
#define GUARD_MELLE_GAMESTATES_GAME_MAP_H

#include <ace/config.h>
#include <ace/utils/bitmap.h>
#include "gamestates/game/bob.h"
#include "gamestates/game/character.h"

#define MAP_LINE_BFR_SIZE (HEX_COUNT_X+2+1)

void mapCreate(void);
void mapDrawHex(tBitMap *pHex, UWORD uwX, UWORD uwY);
void mapDrawAxisHex(tBitMap *pHex, tAxisCoord *pAxis);
void mapPlaceCharacterOnHex(tCharacter *pChar);
void mapCheckAxisConstraints(tAxisCoord *pAxis);

#endif // GUARD_MELLE_GAMESTATES_GAME_MAP_H
