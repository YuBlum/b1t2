#ifndef __SYSTEMS_H__
#define __SYSTEMS_H__

#include "game/entity.h"

void follow_cursor(struct entity *self, float dt);
void keyboard_control(struct entity *self, float dt);
void get_next_position(struct entity *self, float dt);
void collide_with_solids(struct entity *self, float dt);
void move(struct entity *self, float dt);
void camera_follow(struct entity *self, float dt);
void depth_by_y(struct entity *self, float dt);
void move_held(struct entity *self, float dt);
void release_held(struct entity *self, float dt);
void hold_released(struct entity *self, float dt);
void follow(struct entity *self, float dt);
void change_facing(struct entity *self, float dt);
void change_facing_gun(struct entity *self, float dt);
void update_movable_state(struct entity *self, float dt);
void update_cursor_state(struct entity *self, float dt);
void loop_animation(struct entity *self, float dt);
void update_animation(struct entity *self, float dt);
void activate_gun(struct entity *self, float dt);
void deactivate_gun(struct entity *self, float dt);
void update_gun(struct entity *self, float dt);
void render_animation(struct entity *self);
void render_sprite(struct entity *self);
void render_rect(struct entity *self);
void render_radius(struct entity *self);
void render_collider(struct entity *self);

#endif/*__SYSTEMS_H__*/
