#include "game/systems.h"
#include "engine/renderer.h"
#include "engine/window.h"
#include "engine/collision.h"

#define ON_UPDATE_SYSTEM(system, ...) void system(struct entity *self, float dt)
#define ON_RENDER_SYSTEM(system, ...) void system(struct entity *self)

ON_UPDATE_SYSTEM(follow_cursor, FOLLOW_CURSOR) {
  (void)dt;
  self->position = window_get_cursor_position();
}

ON_UPDATE_SYSTEM(keyboard_control, KEYBOARD_CONTROLLED) {
  self->velocity = v2_muls(v2_unit(V2(
    window_is_key_down(K_RIGHT) - window_is_key_down(K_LEFT),
    window_is_key_down(K_UP)    - window_is_key_down(K_DOWN)
  )), self->speed * dt);
}

ON_UPDATE_SYSTEM(move, MOVABLE) {
  (void)dt;
  self->position = v2_add(self->position, self->velocity);
}

ON_UPDATE_SYSTEM(move_held, HOLDING) {
  (void)dt;
  auto target = entity_get_data(self->target);
  target->target_position = V2(self->position.x, self->position.y + (self->size.y + target->size.y) * 0.5f);
}

ON_UPDATE_SYSTEM(release_held, HOLDING) {
  (void)dt;
  if (!window_is_key_press(K_INTERACT)) return;
  entity_remove_flags(self, HOLDING);
  entity_add_flags(self, NOT_HOLDING);
}

ON_UPDATE_SYSTEM(hold_released, NOT_HOLDING) {
  (void)dt;
  if (!window_is_key_press(K_INTERACT)) return;
  auto target = entity_get_data(self->target);
  if (!check_rect_circle(target->position, target->size, self->position, self->interaction_radius)) return;
  entity_remove_flags(self, NOT_HOLDING);
  entity_add_flags(self, HOLDING);
}

ON_UPDATE_SYSTEM(follow, FOLLOW) {
  self->position = v2_lerp(self->position, self->target_position, self->speed * dt);
}

ON_UPDATE_SYSTEM(change_facing, MOVABLE, FACING) {
  (void)dt;
  if      (self->velocity.x > 0.0f) self->scale.x = +1.0f;
  else if (self->velocity.x < 0.0f) self->scale.x = -1.0f;
}

ON_UPDATE_SYSTEM(update_state, STATE_MACHINE) {
  (void)dt;
  auto prv_state = self->state;
  if (self->velocity.x != 0.0f || self->velocity.y != 0.0f) {
    self->state = STM_WALK;
  } else {
    self->state = STM_IDLE;
  }
  if (self->state != prv_state) {
    self->change_frame_timer = 0.0f;
    self->current_frame = 0.0f;
  }
  self->animation = self->state_animation[self->state];
}

ON_UPDATE_SYSTEM(update_animation, RENDER_ANIMATION) {
  auto animation = renderer_animation_get_data(self->animation);
  self->change_frame_timer += dt;
  if (self->change_frame_timer < animation->durations[self->current_frame]) return;
  self->change_frame_timer = 0;
  self->current_frame = (self->current_frame + 1) % animation->frames_amount;
}

ON_RENDER_SYSTEM(render_animation, RENDER_ANIMATION) {
  renderer_request_animation(
    self->animation,
    self->current_frame,
    self->position,
    V2S(0.0f),
    0.0f,
    self->scale,
    WHITE,
    1.0f,
    0.0f
  );
}

ON_RENDER_SYSTEM(render_sprite, RENDER_SPRITE) {
  renderer_request_sprite(
    self->sprite,
    self->position,
    V2S(0.0f),
    0.0f,
    self->scale,
    WHITE,
    1.0f,
    0.0f
  );
}

#if DEV
ON_RENDER_SYSTEM(render_rect, RENDER_RECT) {
  renderer_request_rect(
    self->position,
    self->size,
    GREEN,
    1.0f,
    0.0f
  );
}

ON_RENDER_SYSTEM(render_radius, RENDER_RADIUS) {
  renderer_request_circle(
    self->position,
    self->interaction_radius,
    BLUE,
    0.2f
  );
}
#endif
