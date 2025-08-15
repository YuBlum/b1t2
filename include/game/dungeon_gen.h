#ifndef __DUNGEON_GEN_H__
#define __DUNGEON_GEN_H__

#include "engine/arena.h"

void dungeon_gen_init(struct arena *arena);
void dungeon_gen_update(struct arena *arena);
void dungeon_gen_render(void);

#endif/*__DUNGEON_GEN_H__*/
