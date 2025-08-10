#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "engine/core.h"

enum entity_flag {
  FLAG_ALIVE = 1 << 0,
};

struct entity_handle {
  uint32_t index;
  uint32_t generation;
};

struct entity {
  enum entity_flag flags;
};

void entity_set_flags(struct entity *entity, enum entity_flag flags);
void entity_clear_flags(struct entity *entity, enum entity_flag flags);
bool entity_get_flags(struct entity *entity, enum entity_flag flags);
struct entity *entity_make(void);
void entity_destroy(struct entity *entity);
struct entity *entity_get_data(struct entity_handle handle);
struct entity_handle entity_get_handle(struct entity *entity);
void entity_manager_init(void);

#endif/*__ENTITY_H__*/
