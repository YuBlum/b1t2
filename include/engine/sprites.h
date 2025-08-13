#ifndef __SPRITES_H__
#define __SPRITES_H__

enum sprite {
  SPR_PLAYER = 0,
  SPR_PIXEL,
  SPR_AIM,
  SPR_TEST,
  SPR_FLOWER,
  SPR_GUN,
  SPR_PLAYER_IDLE,
  SPRITES_AMOUNT
};

enum animation {
  ANIM_PLAYER_IDLE = 0,
  ANIM_PLAYER_WALK,
  ANIM_AIM_PRESSED,
  ANIM_AIM_IDLE,
  ANIMATIONS_AMOUNT
};

struct animation_data {
  const float *durations;
  float frame_width;
  uint32_t frames_amount;
  uint32_t first_frame;
  enum sprite sprite;
};

#endif/*__SPRITES_H__*/
