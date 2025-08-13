#ifndef __ATLAS_H__
#define __ATLAS_H__

#include "engine/math.h"
#include "engine/sprites.h"

#define ATLAS_WIDTH 512
#define ATLAS_HEIGHT 64
#define ATLAS_PIXEL_W (1.0f/512)
#define ATLAS_PIXEL_H (1.0f/64)

static const uint8_t g_atlas_data[ATLAS_WIDTH*ATLAS_HEIGHT*4] = {
#embed "imgs/atlas.bin"
};

static const struct v2 g_atlas_sprite_positions[SPRITES_AMOUNT] = {
  { 0.0f * ATLAS_PIXEL_W, 0.0f * ATLAS_PIXEL_H },
  { 456.0f * ATLAS_PIXEL_W, 0.0f * ATLAS_PIXEL_H },
  { 457.0f * ATLAS_PIXEL_W, 0.0f * ATLAS_PIXEL_H },
  { 0.0f * ATLAS_PIXEL_W, 24.0f * ATLAS_PIXEL_H },
  { 16.0f * ATLAS_PIXEL_W, 24.0f * ATLAS_PIXEL_H },
  { 41.0f * ATLAS_PIXEL_W, 24.0f * ATLAS_PIXEL_H },
  { 56.0f * ATLAS_PIXEL_W, 24.0f * ATLAS_PIXEL_H },
};

static const struct v2 g_atlas_sprite_sizes[SPRITES_AMOUNT] = {
  { 456.0f * ATLAS_PIXEL_W, 24.0f * ATLAS_PIXEL_H },
  { 1.0f * ATLAS_PIXEL_W, 1.0f * ATLAS_PIXEL_H },
  { 42.0f * ATLAS_PIXEL_W, 7.0f * ATLAS_PIXEL_H },
  { 16.0f * ATLAS_PIXEL_W, 16.0f * ATLAS_PIXEL_H },
  { 25.0f * ATLAS_PIXEL_W, 24.0f * ATLAS_PIXEL_H },
  { 15.0f * ATLAS_PIXEL_W, 12.0f * ATLAS_PIXEL_H },
  { 96.0f * ATLAS_PIXEL_W, 16.0f * ATLAS_PIXEL_H },
};

static const struct v2 g_atlas_sprite_half_sizes[SPRITES_AMOUNT] = {
  { 228.0f * UNIT_ONE_PIXEL, 12.0f * UNIT_ONE_PIXEL },
  { 0.5f * UNIT_ONE_PIXEL, 0.5f * UNIT_ONE_PIXEL },
  { 21.0f * UNIT_ONE_PIXEL, 3.5f * UNIT_ONE_PIXEL },
  { 8.0f * UNIT_ONE_PIXEL, 8.0f * UNIT_ONE_PIXEL },
  { 12.5f * UNIT_ONE_PIXEL, 12.0f * UNIT_ONE_PIXEL },
  { 7.5f * UNIT_ONE_PIXEL, 6.0f * UNIT_ONE_PIXEL },
  { 48.0f * UNIT_ONE_PIXEL, 8.0f * UNIT_ONE_PIXEL },
};

static const float g_atlas_animation_durations_player_idle[8] = {
  0.3,
  0.1,
  0.15,
  0.15,
  0.3,
  0.15,
  0.15,
  0.1,
};

static const float g_atlas_animation_durations_player_walk[11] = {
  0.08,
  0.08,
  0.08,
  0.08,
  0.08,
  0.08,
  0.08,
  0.08,
  0.08,
  0.08,
  0.08,
};

static const float g_atlas_animation_durations_aim_pressed[3] = {
  0.05,
  0.05,
  0.05,
};

static const float g_atlas_animation_durations_aim_idle[3] = {
  0.05,
  0.05,
  0.05,
};

static const struct animation_data g_atlas_animations[ANIMATIONS_AMOUNT] = {
  {
    .durations = g_atlas_animation_durations_player_idle,
    .frame_width = 24.0f * ATLAS_PIXEL_W,
    .frames_amount = 8,
    .first_frame = 0,
    .sprite = SPR_PLAYER
  },
  {
    .durations = g_atlas_animation_durations_player_walk,
    .frame_width = 24.0f * ATLAS_PIXEL_W,
    .frames_amount = 11,
    .first_frame = 8,
    .sprite = SPR_PLAYER
  },
  {
    .durations = g_atlas_animation_durations_aim_pressed,
    .frame_width = 7.0f * ATLAS_PIXEL_W,
    .frames_amount = 3,
    .first_frame = 0,
    .sprite = SPR_AIM
  },
  {
    .durations = g_atlas_animation_durations_aim_idle,
    .frame_width = 7.0f * ATLAS_PIXEL_W,
    .frames_amount = 3,
    .first_frame = 3,
    .sprite = SPR_AIM
  },
};

#endif/*__ATLAS_H__*/
