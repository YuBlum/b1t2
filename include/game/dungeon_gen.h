#ifndef __DUNGEON_GEN_H__
#define __DUNGEON_GEN_H__

#include "engine/arena.h"

#define MAP_OFFSET 6
#define MAP_W (50+MAP_OFFSET*2)
#define MAP_H (50+MAP_OFFSET*2)

bool dungeon_gen_is_tile_empty(struct v2 position);
void dungeon_gen_init(struct arena *arena);
void dungeon_gen_update(struct arena *arena);
void dungeon_gen_render(void);

#endif/*__DUNGEON_GEN_H__*/
