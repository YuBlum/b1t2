#ifndef __SYSTEMS_H__
#define __SYSTEMS_H__

#include "game/entity.h"

void keyboard_control(struct entity *self, float dt);
void move(struct entity *self, float dt);
void move_held(struct entity *self, float dt);
void release_held(struct entity *self, float dt);
void hold_released(struct entity *self, float dt);
void follow(struct entity *self, float dt);
void update_state(struct entity *self, float dt);
void update_animation(struct entity *self, float dt);
void render_state_animation(struct entity *self);
void render_sprite(struct entity *self);
void render_rect(struct entity *self);
void render_radius(struct entity *self);

#endif/*__SYSTEMS_H__*/
