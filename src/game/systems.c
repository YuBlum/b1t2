#include "game/systems.h"
#include "engine/renderer.h"

#define ON_UPDATE_SYSTEM(system, ...) void system(struct entity *self, float dt)
#define ON_RENDER_SYSTEM(system, ...) void system(struct entity *self)

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

