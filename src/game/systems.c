#include "game/systems.h"
#include "engine/renderer.h"
#include "engine/window.h"
#include "engine/collision.h"

#define ON_UPDATE_SYSTEM(system, ...) void system(struct entity *self, float dt)
#define ON_RENDER_SYSTEM(system, ...) void system(struct entity *self)

ON_UPDATE_SYSTEM(keyboard_control, MOVABLE, KEYBOARD_CONTROLLED) {
  (void)dt;
  self->velocity = v2_muls(v2_unit(V2(
    window_is_key_down(K_RIGHT) - window_is_key_down(K_LEFT),
    window_is_key_down(K_UP)    - window_is_key_down(K_DOWN)
  )), self->speed);
}

ON_UPDATE_SYSTEM(move, MOVABLE) {
  self->position.x += self->velocity.x * dt;
  self->position.y += self->velocity.y * dt;
}

ON_UPDATE_SYSTEM(move_held, HOLDING) {
  (void)dt;
  auto target = entity_get_data(self->target);
  if (target) target->target_position = V2(self->position.x, self->position.y + (self->size.y + target->size.y) * 0.5f);
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
  if (!target || !check_rect_circle(target->position, target->size, self->position, self->interaction_radius)) return;
  entity_remove_flags(self, NOT_HOLDING);
  entity_add_flags(self, HOLDING);
}

ON_UPDATE_SYSTEM(follow, FOLLOW) {
  self->position = v2_lerp(self->position, self->target_position, self->speed * dt);
}

ON_RENDER_SYSTEM(render_sprite, RENDER_SPRITE) {
  renderer_request_sprite(
    self->sprite,
    self->position,
    V2S(0.0f), 
    0.0f,
    V2S(1.0f),
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
