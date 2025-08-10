#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "engine/core.h"
#include "engine/arena.h"
#include "engine/math.h"
#include "engine/sprites.h"

struct color { float r, g, b; };
#define RGB(r, g, b) ((struct color) { r, g, b })
#define WHITE RGB(1.0f, 1.0f, 1.0f)
#define RED   RGB(1.0f, 0.0f, 0.0f)
#define GREEN RGB(0.0f, 1.0f, 0.0f)
#define BLUE  RGB(0.0f, 0.0f, 1.0f)
#define BLACK RGB(0.0f, 0.0f, 0.0f)

bool renderer_make(void);
void renderer_submit(void);

struct v2 renderer_get_offset(void);
void renderer_set_offset(struct v2 offset);

void renderer_request_sprite(enum sprite sprite, struct v2 position, struct v2 origin, float angle, struct v2 scale, struct color color, float opacity, float depth);
void renderer_request_sprite_slice(enum sprite sprite, struct v2u top_left, struct v2u size, struct v2 position, struct v2 origin, float angle, struct v2 scale, struct color color, float opacity, float depth);

#if DEV
void renderer_request_circle(struct v2 position, float radius, struct color color, float opacity);
void renderer_request_rect(struct v2 position, struct v2 size, struct color color, float opacity, float depth);
#endif

#endif/*__RENDERER_H__*/
