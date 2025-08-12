#ifndef __SPRITES_H__
#define __SPRITES_H__

enum sprite {
  SPR_PLAYER = 0,
  SPR_PIXEL,
  SPR_TEST,
  SPR_PLAYER_IDLE,
  SPRITES_AMOUNT
};

struct animation {
  float change_frame_timer; 
  enum animation_index : uint16_t {
    ANIM_PLAYER_IDLE = 0,
    ANIM_PLAYER_WALK,
    ANIMATIONS_AMOUNT,
  } index;
  uint16_t current_frame;
};

#endif/*__SPRITES_H__*/
