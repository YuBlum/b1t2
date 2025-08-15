#include <stdlib.h>
#include <time.h>
#include "engine/arena.h"
#include "engine/window.h"
#include "engine/mixer.h"
#include "engine/renderer.h"
#include "game/global.h"
#include "game/sound.h"
#include "game/entity.h"
#include "game/dungeon_gen.h"

void
test_entities(void) {
  auto cursor = entity_make(RENDER_ANIMATION|STATE_MACHINE|FOLLOW_CURSOR);
  auto player = entity_make(RENDER_ANIMATION|STATE_MACHINE|LOOPABLE|MOVABLE|KEYBOARD_CONTROLLED|HOLDING|DEPTH_BY_Y|HAS_GUN);
  auto flower = entity_make(RENDER_COLLIDER|RENDER_SPRITE|FOLLOW|DEPTH_BY_Y);
  auto gun    = entity_make(NO_FLAGS);

  cursor->state_animation[STM_IDLE]    = ANIM_AIM_IDLE;
  cursor->state_animation[STM_PRESSED] = ANIM_AIM_PRESSED;
  cursor->scale                        = V2S(1.0f);
  cursor->depth                        = -INFINITY;

  player->position                  = V2S(0.0f);
  player->speed                     = 6.0f;
  player->target                    = entity_get_handle(flower);
  player->size                      = V2(1.0f, 1.0f);
  player->interaction_radius        = 1.5f;
  player->state_animation[STM_IDLE] = ANIM_PLAYER_IDLE;
  player->state_animation[STM_WALK] = ANIM_PLAYER_WALK;
  player->scale                     = V2S(1.0f);
  player->gun                       = entity_get_handle(gun);

  flower->position  = player->position;
  flower->sprite    = SPR_FLOWER;
  flower->size      = V2(0.5f, 0.5f);
  flower->speed     = 15.0f;
  flower->scale     = V2S(1.0f);
  flower->origin    = V2(-0.1f, -0.5f);

  gun->sprite = SPR_GUN;
  gun->scale  = V2S(1.0f);
}

static float dt;

bool
game_loop(struct arena *arena) {
  dt = window_frame_begin();
  (void)arena_clear(arena);
  if (window_is_key_down(KEY_EXIT)) window_close();
  global_update(dt);
  //entity_manager_update(dt);
  //entity_manager_render();
  dungeon_gen_update(arena);
  dungeon_gen_render();
  renderer_submit();
  //log_infolf("FPS: %g", 1.0f/dt);
  return window_frame_end();
}

int
main(void) {
  struct arena *arena = arena_make(0, 0);
  //srand(time(0));
  srand(1755281900);
  if (!window_make()) return 1;
  if (!mixer_make()) {
    window_destroy();
    return 1;
  }
  if (!sound_init()) {
    mixer_destroy();
    window_destroy();
    return 1;
  }
  if (!renderer_make()) {
    mixer_destroy();
    window_destroy();
    return 1;
  }
  entity_manager_init();
  global_init();
  test_entities();
  dungeon_gen_init(arena);
  window_run(arena);
  mixer_destroy();
  window_destroy();
  return 0;
}
