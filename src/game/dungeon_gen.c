#include "engine/renderer.h"
#include <string.h>

#define MAP_W 50
#define MAP_H 50
#define MIN_SECTION_SIZE 10
#define ROOMS_AMOUNT 5

enum split_direction {
  SPLIT_HORIZONTAL = 0,
  SPLIT_VERTICAL,
};

struct section {
  struct v2u position;
  struct v2u size;
};

struct bsp_node {
  struct bsp_node *children[2];
  struct section section;
};

struct sections {
  struct section *data;
  uint32_t amount;
  uint32_t capacity;
};

struct bsp_list {
  struct bsp_list *next;
  struct bsp_node *node;
};

static bool map[MAP_W*MAP_H];

static void
map_set_point(uint32_t x, uint32_t y, bool value) {
  if (x >= MAP_W || y >= MAP_H) return;
  map[y*MAP_W+x] = value;
}

static uint32_t
bsp_make_sections(struct arena *arena, struct bsp_node *root, enum split_direction direction, uint32_t current_sections_amount) {
  root->children[0] = arena_push_type(arena, false, struct bsp_node);
  root->children[1] = arena_push_type(arena, false, struct bsp_node);
  if (!root->children[0] || !root->children[1]) {
    log_errorlf("%s: not enough memory for sections", __func__);
    exit(1);
  }
  struct v2u size[2];
  struct v2u offset;
  enum split_direction next_direction[2];
  float threshold = 0.0f, split_point;
  switch (direction) {
    case SPLIT_HORIZONTAL: {
      size[0].y = root->section.size.y;
      size[1].y = root->section.size.y;
      do {
        split_point = randf_from_to(0.0f + threshold, 1.0f - threshold);
        size[0].x = (uint32_t)(root->section.size.x * split_point);
        size[1].x = root->section.size.x - size[0].x;
        threshold += 0.01f;
      } while ((size[0].x < MIN_SECTION_SIZE || size[1].x < MIN_SECTION_SIZE) && threshold < 0.5f);
      offset  = V2U(size[0].x, 0);
    } break;
    case SPLIT_VERTICAL: {
      size[0].x = root->section.size.x;
      size[1].x = root->section.size.x;
      do {
        split_point = randf_from_to(0.0f + threshold, 1.0f - threshold);
        size[0].y = (uint32_t)(root->section.size.y * split_point);
        size[1].y = root->section.size.y - size[0].y;
        threshold += 0.01f;
      } while ((size[0].y < MIN_SECTION_SIZE || size[1].y < MIN_SECTION_SIZE) && threshold < 0.5f);
      offset = V2U(0, size[0].y);
    } break;
  }
  root->children[0]->section.size = size[0];
  root->children[1]->section.size = size[1];
  root->children[0]->section.position = root->section.position;
  root->children[1]->section.position = v2u_add(root->section.position, offset);
  for (uint32_t i = 0; i < 2; i++) {
    bool split = false;
    if (size[i].x >= MIN_SECTION_SIZE*2 && size[i].y >= MIN_SECTION_SIZE*2) {
      if (size[i].x / (float)size[i].y >= 1.25) {
        next_direction[i] = SPLIT_VERTICAL;
        split = true;
      } else if (size[i].y / (float)size[i].x >= 1.25) {
        next_direction[i] = SPLIT_HORIZONTAL;
        split = true;
      } else {
        next_direction[i] = rand()%2;
        split = true;
      }
    } else if (size[i].x >= MIN_SECTION_SIZE*2) {
      next_direction[i] = SPLIT_HORIZONTAL;
      split = true;
    } else if (size[i].y >= MIN_SECTION_SIZE*2) {
      next_direction[i] = SPLIT_VERTICAL;
      split = true;
    }
    if (split) {
      current_sections_amount = bsp_make_sections(arena, root->children[i], next_direction[i], current_sections_amount);
    } else {
      current_sections_amount++;
      /*
      for (uint32_t j = 0; j < root->children[i]->section.size.y; j++) {
        map_set_point(root->children[i]->section.position.x, root->children[i]->section.position.y + j, true);
        map_set_point(root->children[i]->section.position.x + root->children[i]->section.size.x-1, root->children[i]->section.position.y + j, true);
      }
      for (uint32_t j = 0; j < root->children[i]->section.size.x; j++) {
        map_set_point(root->children[i]->section.position.x + j, root->children[i]->section.position.y, true);
        map_set_point(root->children[i]->section.position.x + j, root->children[i]->section.position.y + root->children[i]->section.size.y-1, true);
      }
      */
    }
  }
  return current_sections_amount;
}

static void
bsp_get_sections(struct bsp_node *node, struct sections *sections) {
  bool is_section = true;
  if (node->children[0]) {
    bsp_get_sections(node->children[0], sections);
    is_section = false;
  }
  if (node->children[1]) {
    bsp_get_sections(node->children[1], sections);
    is_section = false;
  }
  if (!is_section) return;
  if (sections->amount >= sections->capacity) {
    log_errorlf("%s: unreachable", __func__);
    exit(1);
  }
  sections->data[sections->amount++] = node->section;
}

void
dungeon_gen_init(struct arena *arena) {
  memset(map, 0, sizeof (map));
  struct bsp_node root = {
    .section = {
      .position = { 0, 0 },
      .size     = { MAP_W, MAP_H }
    }
  };
  auto sections_capacity = bsp_make_sections(arena, &root, randf() < 0.5f ? SPLIT_VERTICAL : SPLIT_HORIZONTAL, 0);
  struct sections sections = {
    .data = arena_push_array(arena, true, struct section, sections_capacity),
    .amount = 0,
    .capacity = sections_capacity
  };
  auto indices = arena_push_array(arena, true, uint32_t, sections_capacity);
  if (!sections.data || !indices) {
    log_errorlf("%s: not enough memory for sections", __func__);
    exit(1);
  }
  bsp_get_sections(&root, &sections);
  if (sections.capacity != sections.amount) {
    log_errorlf("%s: unreachable", __func__);
    exit(1);
  }
  for (uint32_t i = 0; i < sections.amount; i++) indices[i] = i;
  for (uint32_t i = 0; i < ROOMS_AMOUNT && sections.amount; i++) {
    uint32_t idx = rand() % sections.amount;
    struct v2u position = { sections.data[idx].position.x + 2, sections.data[idx].position.y + 2 };
    struct v2u size     = { sections.data[idx].size    .x - 2, sections.data[idx].size    .y - 2 };
    for (uint32_t y = 0; y < size.y; y++) {
      for (uint32_t x = 0; x < size.x; x++) {
        map_set_point(position.x + x, position.y + y, true);
      }
    }
    sections.data[idx] = sections.data[--sections.amount];
  }
}

void
dungeon_gen_render(void) {
  for (uint32_t y = 0; y < MAP_H; y++) {
    for (uint32_t x = 0; x < MAP_W; x++) {
      struct v2 position = {
        (MAP_H * 0.5f - y) * UNIT_ONE_PIXEL,
        (x - MAP_W * 0.5f) * UNIT_ONE_PIXEL,
      };
      renderer_request_point(position, map[y*MAP_W+x] ? GREEN : RED, 1.0f, 0.0f);
    }
  }
}
