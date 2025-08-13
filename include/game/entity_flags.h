#ifndef __ENTITY_FLAGS_H__
#define __ENTITY_FLAGS_H__

enum entity_flag {
  NO_FLAGS = 0,
  ALIVE = 1 << 0,
  RENDER_SPRITE = 1 << 1,
  RENDER_ANIMATION = 1 << 2,
  RENDER_RADIUS = 1 << 3,
  RENDER_COLLIDER = 1 << 4,
  MOVABLE = 1 << 5,
  KEYBOARD_CONTROLLED = 1 << 6,
  HOLDING = 1 << 7,
  NOT_HOLDING = 1 << 8,
  RENDER_RECT = 1 << 9,
  FOLLOW = 1 << 10,
  STATE_MACHINE = 1 << 11,
  FACING = 1 << 12,
  FOLLOW_CURSOR = 1 << 13,
  LOOPABLE = 1 << 14,
  DEPTH_BY_Y = 1 << 15,
  HAS_GUN = 1 << 16,
  HOLDING_GUN = 1 << 17,
};

#endif/*__ENTITY_FLAGS_H__*/
