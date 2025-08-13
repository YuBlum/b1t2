#ifndef __ENTITY_FLAGS_H__
#define __ENTITY_FLAGS_H__

enum entity_flag {
  NO_FLAGS = 0,
  ALIVE = 1 << 0,
  RENDER_SPRITE = 1 << 1,
  RENDER_ANIMATION = 1 << 2,
  RENDER_RADIUS = 1 << 3,
  MOVABLE = 1 << 4,
  KEYBOARD_CONTROLLED = 1 << 5,
  HOLDING = 1 << 6,
  NOT_HOLDING = 1 << 7,
  RENDER_RECT = 1 << 8,
  FOLLOW = 1 << 9,
  STATE_MACHINE = 1 << 10,
  FACING = 1 << 11,
  FOLLOW_CURSOR = 1 << 12,
};

#endif/*__ENTITY_FLAGS_H__*/
