#include "game/systems.h"
#include "game/global.h"
#include "engine/renderer.h"
#include "engine/window.h"
#include "engine/collision.h"
#include "game/dungeon_gen.h"

#define ON_UPDATE_SYSTEM(system, ...) void system(struct entity *self, float dt)
#define ON_RENDER_SYSTEM(system, ...) void system(struct entity *self)

ON_UPDATE_SYSTEM(follow_cursor, FOLLOW_CURSOR) {
  (void)dt;
  self->position = window_get_cursor_position();
}

ON_UPDATE_SYSTEM(keyboard_control, KEYBOARD_CONTROLLED) {
  (void)dt;
  self->direction = v2_unit(V2(
    window_is_key_down(KEY_RIGHT) - window_is_key_down(KEY_LEFT),
    window_is_key_down(KEY_UP)    - window_is_key_down(KEY_DOWN)
  ));
}

ON_UPDATE_SYSTEM(get_next_position, MOVABLE) {
  self->next_position = v2_add(self->position, v2_muls(self->direction, self->speed * dt));
}

ON_UPDATE_SYSTEM(collide_with_solids, COLLIDABLE) {
  (void)dt;
  auto x_test = V2(self->next_position.x, self->position.y);
  auto y_test = V2(self->position.x, self->next_position.y);
  float tile_position_x, tile_position_y;
  bool tile_found_x = false, tile_found_y = false;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      struct v2 tile_position = V2(roundf(self->position.x) + i, roundf(self->position.y) + j);
      if (!dungeon_gen_is_tile_empty(tile_position)) continue;
      if (check_rect_rect(x_test, self->size, tile_position, V2S(1.0f))) {
        tile_found_x = true;
        tile_position_x = tile_position.x;
      }
      if (check_rect_rect(y_test, self->size, tile_position, V2S(1.0f))) {
        tile_found_y = true;
        tile_position_y = tile_position.y;
      }
    }
  }
  if (tile_found_x) {
    self->next_position.x = resolve_rect_rect_axis(self->position.x, self->size.x, tile_position_x, 1.0f);
    self->direction.x = 0.0f;
  }
  if (tile_found_y) {
    self->next_position.y = resolve_rect_rect_axis(self->position.y, self->size.y, tile_position_y, 1.0f);
    self->direction.y = 0.0f;
  }
}

ON_UPDATE_SYSTEM(move, MOVABLE) {
  (void)dt;
  self->position = self->next_position;
}

#define CAM_EPSILON 0.01f
#define CAM_MOVE_BY_FACTOR 0.99f

ON_UPDATE_SYSTEM(camera_follow, CAMERA_FOLLOW) {
  auto percent  = 1.0f - powf(1.0f - CAM_MOVE_BY_FACTOR, dt);
  auto next_pos = v2_lerp(renderer_get_offset(), self->position, percent);
  if (fabsf(next_pos.x - self->position.x) <= CAM_EPSILON) next_pos.x = self->position.x;
  if (fabsf(next_pos.y - self->position.y) <= CAM_EPSILON) next_pos.y = self->position.y;
  renderer_set_offset(next_pos);
}

ON_UPDATE_SYSTEM(depth_by_y, DEPTH_BY_Y) {
  (void)dt;
  self->depth = self->position.y - self->size.y * 0.5f;
}

ON_UPDATE_SYSTEM(move_held, HOLDING) {
  (void)dt;
  auto target = entity_get_data(self->target);
  target->target_position = V2(
    self->position.x,
    self->position.y + (self->size.y + target->size.y) * 0.5f + sinf(global.time * 5.0f) * 0.2f
  );
  target->scale.x = self->scale.x;
  target->depth = self->depth + 0.1f;
}

ON_UPDATE_SYSTEM(release_held, HOLDING) {
  (void)dt;
  if (!window_is_key_press(KEY_INTERACT)) return;
  entity_remove_flags(self, HOLDING);
  entity_add_flags(self, NOT_HOLDING|DEPTH_BY_Y);
}

ON_UPDATE_SYSTEM(hold_released, NOT_HOLDING) {
  (void)dt;
  if (!window_is_key_press(KEY_INTERACT)) return;
  auto target = entity_get_data(self->target);
  if (!check_rect_circle(target->position, target->size, self->position, self->interaction_radius)) return;
  entity_remove_flags(self, NOT_HOLDING|DEPTH_BY_Y);
  entity_add_flags(self, HOLDING);
}

ON_UPDATE_SYSTEM(follow, FOLLOW) {
  self->position = v2_lerp(self->position, self->target_position, self->speed * dt);
}

ON_UPDATE_SYSTEM(change_facing, NOT_HOLDING_GUN) {
  (void)dt;
  if      (self->direction.x > 0.0f) self->scale.x = +1.0f;
  else if (self->direction.x < 0.0f) self->scale.x = -1.0f;
}

ON_UPDATE_SYSTEM(change_facing_gun, HOLDING_GUN) {
  (void)dt;
  auto cursor = window_get_cursor_position();
  if      (self->position.x < cursor.x) self->scale.x = +1.0f;
  else if (self->position.x > cursor.x) self->scale.x = -1.0f;
}

ON_UPDATE_SYSTEM(update_movable_state, MOVABLE, STATE_MACHINE) {
  (void)dt;
  auto prv_state = self->state;
  if (self->direction.x != 0.0f || self->direction.y != 0.0f) {
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

ON_UPDATE_SYSTEM(update_cursor_state, FOLLOW_CURSOR, STATE_MACHINE) {
  (void)dt;
  auto prv_state = self->state;
  if (window_is_button_down(BTN_LEFT)) {
    self->state = STM_PRESSED;
  } else {
    self->state = STM_IDLE;
  }
  if (self->state != prv_state) {
    self->change_frame_timer = 0.0f;
    self->current_frame = 0.0f;
  }
  self->animation = self->state_animation[self->state];
}

ON_UPDATE_SYSTEM(loop_animation, RENDER_ANIMATION, LOOPABLE) {
  (void)dt;
  auto animation = renderer_animation_get_data(self->animation);
  if (self->current_frame >= animation->frames_amount-1 && self->previous_frame >= animation->frames_amount-1) self->current_frame = 0;
}

ON_UPDATE_SYSTEM(update_animation, RENDER_ANIMATION) {
  auto animation = renderer_animation_get_data(self->animation);
  self->change_frame_timer += dt;
  if (self->change_frame_timer < animation->durations[self->current_frame]) return;
  self->change_frame_timer = 0;
  self->previous_frame = self->current_frame;
  self->current_frame++;
  if (self->current_frame > animation->frames_amount-1) self->current_frame = animation->frames_amount-1;
}

ON_UPDATE_SYSTEM(activate_gun, HAS_GUN, NOT_HOLDING) {
  (void)dt;
  auto gun = entity_get_data(self->gun);
  entity_remove_flags(self, NOT_HOLDING_GUN);
  entity_add_flags(self, HOLDING_GUN);
  entity_add_flags(gun, RENDER_SPRITE);
}

ON_UPDATE_SYSTEM(deactivate_gun, HAS_GUN, HOLDING) {
  (void)dt;
  auto gun = entity_get_data(self->gun);
  entity_add_flags(self, NOT_HOLDING_GUN);
  entity_remove_flags(self, HOLDING_GUN);
  entity_remove_flags(gun, RENDER_SPRITE);
}

ON_UPDATE_SYSTEM(update_gun, HOLDING_GUN) {
  (void)dt;
  auto gun       = entity_get_data(self->gun);
  gun->depth     = self->depth - 0.1f;
  gun->angle     = v2_to_angle(v2_sub(window_get_cursor_position(), self->position));
  gun->scale.y   = gun->angle > -(PI*0.5f) && gun->angle < (PI*0.5f) ? 1.0f : -1.0f;
  gun->direction = v2_from_angle(gun->angle);
  gun->position  = v2_add(self->position, gun->direction);
  if (window_is_button_press(BTN_LEFT)) {
    auto bullet       = entity_make(MOVABLE|RENDER_RECT);
    bullet->position  = v2_add(gun->position, v2_muls(v2_from_angle(gun->angle), 0.3f));
    bullet->direction = gun->direction;
    bullet->speed     = 40.0f;
    bullet->size      = V2S(2*UNIT_ONE_PIXEL);
    bullet->color     = WHITE;
  } else if (window_is_button_down(BTN_LEFT)) {
    gun->position = v2_sub(gun->position, v2_muls(gun->direction, 0.3f));
  }
}

ON_RENDER_SYSTEM(render_animation, RENDER_ANIMATION) {
  renderer_request_animation(
    self->animation,
    self->current_frame,
    self->position,
    .origin  = self->origin,
    .angle   = self->angle,
    .scale   = self->scale,
    .depth   = self->depth
  );
}

ON_RENDER_SYSTEM(render_sprite, RENDER_SPRITE) {
  renderer_request_sprite(
    self->sprite,
    self->position,
    .origin  = self->origin,
    .angle   = self->angle,
    .scale   = self->scale,
    .depth   = self->depth
  );
}

ON_RENDER_SYSTEM(render_rect, RENDER_RECT) {
  renderer_request_rect(
    self->position,
    self->size,
    self->color,
    1.0f,
    self->depth
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

ON_RENDER_SYSTEM(render_collider, RENDER_COLLIDER) {
  if (!global.show_colliders) return;
  renderer_request_rect(
    self->position,
    self->size,
    GREEN,
    0.4f,
    -100.0f
  );
}
