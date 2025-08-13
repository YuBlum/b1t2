#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "engine/core.h"

enum key : uint8_t {
  K_LEFT = 0,
  K_RIGHT,
  K_UP,
  K_DOWN,
  K_INTERACT,
  K_RESTART,
  K_COLLIDERS,
  K_EXIT,
  KEY_AMOUNT,
};

bool window_make();
void window_destroy(void);
void window_close();
void window_run(void);
float window_frame_begin(void);
bool window_frame_end(void);
bool window_is_key_press(enum key key);
bool window_is_key_down(enum key key);
bool window_is_key_up(enum key key);
bool window_is_key_release(enum key key);
struct v2 window_get_cursor_position(void);

bool game_loop(void);

#endif/*__WINDOW_H__*/
