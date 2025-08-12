#ifndef __ENTITY_FLAGS_H__
#define __ENTITY_FLAGS_H__

enum entity_flag {
  NO_FLAGS = 0,
  ALIVE = 1 << 0,
  RENDER_SPRITE = 1 << 1,
  MOVING = 1 << 2,
  NOT_MOVING = 1 << 3,
  KEYBOARD_CONTROLLED = 1 << 4,
  HOLDING = 1 << 5,
  NOT_HOLDING = 1 << 6,
  RENDER_RECT = 1 << 7,
  FOLLOW = 1 << 8,
  RENDER_RADIUS = 1 << 9,
  STATE_MACHINE = 1 << 10,
  FACING = 1 << 11,
};

#endif/*__ENTITY_FLAGS_H__*/
