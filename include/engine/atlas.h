#ifndef __ATLAS_H__
#define __ATLAS_H__

#include "engine/math.h"
#include "engine/sprites.h"

#define ATLAS_WIDTH 256
#define ATLAS_HEIGHT 64
#define ATLAS_PIXEL_W (1.0f/256)
#define ATLAS_PIXEL_H (1.0f/64)

static const uint8_t g_atlas_data[ATLAS_WIDTH*ATLAS_HEIGHT*4] = {
#embed "imgs/atlas.bin"
};

static const struct v2 g_atlas_sprite_positions[SPRITES_AMOUNT] = {
  { 0.0f * ATLAS_PIXEL_W, 0.0f * ATLAS_PIXEL_H },
  { 168.0f * ATLAS_PIXEL_W, 0.0f * ATLAS_PIXEL_H },
  { 169.0f * ATLAS_PIXEL_W, 0.0f * ATLAS_PIXEL_H },
  { 0.0f * ATLAS_PIXEL_W, 24.0f * ATLAS_PIXEL_H },
};

static const struct v2 g_atlas_sprite_sizes[SPRITES_AMOUNT] = {
  { 168.0f * ATLAS_PIXEL_W, 24.0f * ATLAS_PIXEL_H },
  { 1.0f * ATLAS_PIXEL_W, 1.0f * ATLAS_PIXEL_H },
  { 16.0f * ATLAS_PIXEL_W, 16.0f * ATLAS_PIXEL_H },
  { 96.0f * ATLAS_PIXEL_W, 16.0f * ATLAS_PIXEL_H },
};

static const struct v2 g_atlas_sprite_half_sizes[SPRITES_AMOUNT] = {
  { 84.0f * UNIT_ONE_PIXEL, 12.0f * UNIT_ONE_PIXEL },
  { 0.5f * UNIT_ONE_PIXEL, 0.5f * UNIT_ONE_PIXEL },
  { 8.0f * UNIT_ONE_PIXEL, 8.0f * UNIT_ONE_PIXEL },
  { 48.0f * UNIT_ONE_PIXEL, 8.0f * UNIT_ONE_PIXEL },
};

static const float g_atlas_animation_durations_player_idle[3] = {
  300.0f,
  100.0f,
  150.0f,
};

static const float g_atlas_animation_durations_player_walk[2] = {
  150.0f,
  150.0f,
};

static const struct animation_data {
  const float *durations;
  float frame_width;
  uint32_t frames_amount;
} g_atlas_animation_frames[ANIMATIONS_AMOUNT] = {
  {
    .durations = g_atlas_animation_durations_player_idle,
    .frame_width = 24.0f * ATLAS_PIXEL_W,
    .frames_amount = 3
  },
  {
    .durations = g_atlas_animation_durations_player_walk,
    .frame_width = 24.0f * ATLAS_PIXEL_W,
    .frames_amount = 2
  },
};

#endif/*__ATLAS_H__*/
