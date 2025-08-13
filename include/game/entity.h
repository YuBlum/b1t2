#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "engine/core.h"
#include "engine/math.h"
#include "engine/sprites.h"

enum entity_state {
  STM_IDLE = 0,
  STM_WALK,
  STM_PRESSED = STM_WALK,
  STM_AMOUNT
};

#include "game/entity_flags.h"

struct entity_handle {
  uint32_t index;
  uint32_t generation;
};
/* bascially the generations starts to be valid at number one
 * because of this the 'ENTITY_HANDLE_NONE' would just be valid if the entity specifically at index 0
 * is at its 4294967296th generation, what is highly unlikely to happen */
#define ENTITY_NONE ((struct entity_handle){0})

struct entity {
  enum entity_flag flags;
  enum entity_flag next_flags;
  struct v2 target_position;
  struct v2 position;
  struct v2 direction;
  struct v2 size;
  struct v2 scale;
  struct v2 origin;
  struct entity_handle target;
  struct entity_handle gun;
  float depth;
  float angle;
  float speed;
  float interaction_radius;
  enum sprite sprite;
  enum animation animation;
  enum animation state_animation[STM_AMOUNT];
  float change_frame_timer;
  uint32_t current_frame;
  enum entity_state state;
};

static inline void
entity_change_state(struct entity *entity, enum entity_state state) {
  entity->state = state;
  entity->change_frame_timer = 0.0f;
  entity->current_frame = 0.0f;
  entity->animation = entity->state_animation[state];
}

void entity_add_flags(struct entity *entity, enum entity_flag flags);
void entity_remove_flags(struct entity *entity, enum entity_flag flags);
bool entity_get_flags(struct entity *entity, enum entity_flag flags);
struct entity *entity_make(enum entity_flag flags);
void entity_destroy(struct entity *entity);
struct entity *entity_get_data(struct entity_handle handle);
struct entity_handle entity_get_handle(struct entity *entity);
void entity_manager_init(void);
void entity_manager_update(float dt);
void entity_manager_render(void);

#endif/*__ENTITY_H__*/
