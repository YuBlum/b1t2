#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "engine/core.h"
#include "engine/math.h"
#include "engine/sprites.h"

enum entity_flag {
  NO_FLAGS      = 0,
  ALIVE         = 1 << 0,
  RENDER_SPRITE = 1 << 1,
};

struct entity_handle {
  uint32_t index;
  uint32_t generation;
};

struct entity {
  enum entity_flag flags;
  struct v2 position;
  struct v2 velocity;
  enum sprite sprite;
};

void entity_set_flags(struct entity *entity, enum entity_flag flags);
void entity_clear_flags(struct entity *entity, enum entity_flag flags);
bool entity_get_flags(struct entity *entity, enum entity_flag flags);
struct entity *entity_make(enum entity_flag flags);
void entity_destroy(struct entity *entity);
struct entity *entity_get_data(struct entity_handle handle);
struct entity_handle entity_get_handle(struct entity *entity);
void entity_manager_init(void);
void entity_manager_update(float dt);
void entity_manager_render(void);

#endif/*__ENTITY_H__*/
