#include "game/entity.h"

#define ENTITY_CAP 10

static struct entity_manager {
  struct entity *cached[ENTITY_CAP]; 
  struct entity  data[ENTITY_CAP];
  uint32_t       generations[ENTITY_CAP];
  uint32_t       cached_index[ENTITY_CAP]; 
  uint32_t       free_list[ENTITY_CAP];
  uint32_t       cached_amount;
  uint32_t       free_list_amount;
} entities;

void
entity_set_flags(struct entity *entity, enum entity_flag flags) {
  entity->flags |= flags;
}

void
entity_clear_flags(struct entity *entity, enum entity_flag flags) {
  entity->flags &= ~flags;
}

bool
entity_get_flags(struct entity *entity, enum entity_flag flags) {
  return (entity->flags & flags) == flags;
}

struct entity *
entity_make(void) {
  #if DEV
  if (!entities.free_list_amount) {
    log_errorlf("%s: trying to make too many entities", __func__);
    return 0;
  }
  #endif
  auto index = entities.free_list[--entities.free_list_amount];
  entities.generations[index]++;
  auto entity = &entities.data[index];
  entities.cached_index[index] = entities.cached_amount;
  entities.cached[entities.cached_amount++] = entity;
  entity->flags = 0;
  entity_set_flags(entity, FLAG_ALIVE);
  return entity;
}

void
entity_destroy(struct entity *entity) {
  auto index = (uint32_t)(entity - entities.data);
  #if DEV
  if (index >= ENTITY_CAP) {
    log_errorlf("%s: invalid entity", __func__);
    return;
  }
  #endif
  if (!entity_get_flags(entity, FLAG_ALIVE)) return;
  entity_clear_flags(entity, FLAG_ALIVE);
  entities.free_list[entities.free_list_amount++] = index;
  entities.cached[entities.cached_index[index]] = entities.cached[--entities.cached_amount];
}

struct entity *
entity_get_data(struct entity_handle handle) {
  if (handle.index > ENTITY_CAP || handle.generation != entities.generations[handle.index]) return 0;
  auto e = &entities.data[handle.index];
  return entity_get_flags(e, FLAG_ALIVE) ? e : 0;
}

struct entity_handle
entity_get_handle(struct entity *entity) {
  struct entity_handle handle;
  handle.index = (uint32_t)(entity - entities.data);
  #if DEV
  if (handle.index >= ENTITY_CAP) {
    log_errorlf("%s: invalid entity", __func__);
    handle.index = 0;
    handle.generation = (uint32_t)-1;
    return handle;
  }
  #endif
  handle.generation = entities.generations[handle.index];
  return handle;
}

void
entity_manager_init(void) {
  entities.free_list_amount = 0;
  for (int64_t i = ENTITY_CAP - 1; i >= 0; i--) {
    entities.free_list[entities.free_list_amount++] = i;
  }
}
