#include <stdlib.h>
#include <time.h>
#include "engine/arena.h"
#include "engine/window.h"
#include "engine/mixer.h"
#include "engine/renderer.h"
#include "game/global.h"
#include "game/sound.h"
#include "game/entity.h"

#define WINDOW_W (GAME_W_PIXEL * GAME_S)
#define WINDOW_H (GAME_H_PIXEL * GAME_S)

void
test_entities(void) {
  auto player = entity_make(RENDER_SPRITE|MOVABLE|KEYBOARD_CONTROLLED|HOLDING);
  auto flower = entity_make(RENDER_RECT);

  player->position = V2S(0.0f);
  player->velocity = V2S(0.0f);
  player->speed    = 10.0f;
  player->sprite   = SPR_TEST;
  player->target   = entity_get_handle(flower);
  player->size     = V2(1.0f, 1.0f);

  flower->position = player->position;
  flower->size     = V2(0.5f, 0.5f);
}

int
main(void) {
  srand(time(0));
  if (!window_make(WINDOW_W, WINDOW_H)) return 1;
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
  float dt = window_get_delta_time(); /* call just to setup timer */
  while (window_is_running()) {
    dt = window_get_delta_time();
    if (window_is_key_down(K_EXIT)) window_close();
    entity_manager_update(dt);
    entity_manager_render();
    renderer_submit();
  }
  mixer_destroy();
  window_destroy();
  return 0;
}
