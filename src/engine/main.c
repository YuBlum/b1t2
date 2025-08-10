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

int
main(void) {
  srand(time(0));
  struct arena *tmp_arena = arena_make(0, 0);
  if (!tmp_arena) {
    log_errorl("couldn't make main temporary arena");
    return 1;
  }
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
  float dt = window_get_delta_time(); /* call just to setup timer */
  while (window_is_running()) {
    dt = window_get_delta_time();
    (void)dt;
    if (window_is_key_down(K_EXIT)) window_close();
    renderer_request_sprite(SPR_PLAYER, V2S(0.0f), V2S(0.0f), 0.0f, V2S(1.0f), WHITE, 1.0f, 0.0f);
    renderer_submit();
    arena_clear(tmp_arena);
  }
  mixer_destroy();
  window_destroy();
  return 0;
}
