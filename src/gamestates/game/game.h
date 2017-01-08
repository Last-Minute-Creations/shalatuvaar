#ifndef GUARD_MELEE_GAMESTATES_GAME_H
#define GUARD_MELEE_GAMESTATES_GAME_H

#include <ace/config.h>
#include <ace/utils/bitmapmask.h>
#include <ace/managers/viewport/simplebuffer.h>

void gsGameCreate(void);
void gsGameLoop(void);
void gsGameDestroy(void);

extern tBitMap *g_pHexGrn;
extern tBitmapMask *g_pHexMask;
extern tSimpleBufferManager *g_pGameBuffer;

#endif // GUARD_MELEE_GAMESTATES_GAME_H
