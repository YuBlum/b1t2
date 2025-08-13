#include <stdlib.h>
#include <time.h>
#include "engine/arena.h"
#include "engine/window.h"
#include "engine/mixer.h"
#include "engine/renderer.h"
#include "game/global.h"
#include "game/sound.h"
#include "game/entity.h"

void
test_entities(void) {
  auto cursor = entity_make(RENDER_ANIMATION|FOLLOW_CURSOR);
  auto player = entity_make(RENDER_ANIMATION|STATE_MACHINE|MOVABLE|KEYBOARD_CONTROLLED|HOLDING|FACING);
  auto flower = entity_make(RENDER_RECT|FOLLOW);

  cursor->animation = ANIM_AIM_IDLE;
  cursor->scale     = V2S(1.0f);

  player->position                  = V2S(0.0f);
  player->speed                     = 6.0f;
  player->target                    = entity_get_handle(flower);
  player->size                      = V2(1.0f, 1.0f);
  player->interaction_radius        = 1.5f;
  player->state_animation[STM_IDLE] = ANIM_PLAYER_IDLE;
  player->state_animation[STM_WALK] = ANIM_PLAYER_WALK;
  player->scale                     = V2S(1.0f);

  flower->position = player->position;
  flower->size     = V2(0.5f, 0.5f);
  flower->speed    = 15.0f;
}

static float dt;

bool
game_loop(void) {
  dt = window_frame_begin();
  if (window_is_key_down(K_EXIT)) window_close();
  entity_manager_update(dt);
  entity_manager_render();
  renderer_submit();
  //log_infolf("FPS: %g", 1.0f/dt);
  return window_frame_end();
}

int
main(void) {
  srand(time(0));
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
  window_run();
  mixer_destroy();
  window_destroy();
  return 0;
}
