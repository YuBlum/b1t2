#include "engine/renderer.h"
#include "engine/window.h"
#include <string.h>

#define MAP_W 50
#define MAP_H 50
#define MIN_SECTION_SIZE 10
#define ROOMS_CAP 10
#define MIN_ROOM_SIZE 5

enum split_direction {
  SPLIT_HORIZONTAL = 0,
  SPLIT_VERTICAL,
};

enum room_surrounds {
  ROOM_UP = 0,
  ROOM_LEFT,
  ROOM_DOWN,
  ROOM_RIGHT,
  ROOM_SURROUNDS_AMOUNT
};

struct room {
  struct room *next;
  struct v2u center;
  struct v2u size;
};

struct section {
  struct v2u top_left;
  struct v2u size;
};

struct bsp_node {
  struct bsp_node *children[2];
  struct room *room;
  struct section section;
};

struct bsp_leafs {
  struct bsp_node **data;
  uint32_t amount;
  uint32_t capacity;
};

struct bsp_list {
  struct bsp_list *next;
  struct bsp_node *node;
};

static struct room g_rooms[ROOMS_CAP];
static uint32_t    g_rooms_amount;
static bool map[MAP_W*MAP_H];

static inline struct v2u room_get_top(uint32_t i)    { return V2U(g_rooms[i].center.x, g_rooms[i].center.y + g_rooms[i].size.y/2); }
static inline struct v2u room_get_left(uint32_t i)   { return V2U(g_rooms[i].center.x - g_rooms[i].size.x/2, g_rooms[i].center.y); }
static inline struct v2u room_get_bottom(uint32_t i) { return V2U(g_rooms[i].center.x, g_rooms[i].center.y - g_rooms[i].size.y/2); }
static inline struct v2u room_get_right(uint32_t i)  { return V2U(g_rooms[i].center.x + g_rooms[i].size.x/2, g_rooms[i].center.y); }

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
  root->children[0]->section.top_left = root->section.top_left;
  root->children[1]->section.top_left = v2u_add(root->section.top_left, offset);
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
    current_sections_amount = split ? bsp_make_sections(arena, root->children[i], next_direction[i], current_sections_amount)
                                    : current_sections_amount + 1;
  }
  return current_sections_amount;
}

static void
bsp_get_leafs(struct bsp_node *node, struct bsp_leafs *leafs) {
  bool is_leaf = true;
  if (node->children[0]) {
    bsp_get_leafs(node->children[0], leafs);
    is_leaf = false;
  }
  if (node->children[1]) {
    bsp_get_leafs(node->children[1], leafs);
    is_leaf = false;
  }
  if (!is_leaf) return;
  if (leafs->amount >= leafs->capacity) {
    log_errorlf("%s: unreachable", __func__);
    exit(1);
  }
  leafs->data[leafs->amount++] = node;
}

static void
carve_corridor(struct room *a, struct room *b) {
  uint32_t x0 = a->center.x, y0 = a->center.y,
           x1 = b->center.x, y1 = b->center.y;
  if (rand() % 2) {
    for (uint32_t x = MIN(x0,x1); x <= MAX(x0,x1); x++) map_set_point(x, y0, true);
    for (uint32_t y = MIN(y0,y1); y <= MAX(y0,y1); y++) map_set_point(x1, y, true);
  } else {
    for (uint32_t y = MIN(y0,y1); y <= MAX(y0,y1); y++) map_set_point(x0, y, true);
    for (uint32_t x = MIN(x0,x1); x <= MAX(x0,x1); x++) map_set_point(x, y1, true);
  }
}

void
dungeon_gen_init(struct arena *arena) {
  memset(map, 0, sizeof (map));
  struct bsp_node root = {
    .section = {
      .top_left = { 0, 0 },
      .size     = { MAP_W, MAP_H }
    }
  };
  struct bsp_leafs leafs;
  leafs.capacity = bsp_make_sections(arena, &root, randf() < 0.5f ? SPLIT_VERTICAL : SPLIT_HORIZONTAL, 0);
  leafs.data     = arena_push_array(arena, true, struct bsp_node *, leafs.capacity);
  leafs.amount   = 0;
  if (!leafs.data) {
    log_errorlf("%s: ran out of memory", __func__);
    exit(1);
  }
  bsp_get_leafs(&root, &leafs);
  if (leafs.capacity != leafs.amount) {
    log_errorlf("%s: unreachable", __func__);
    exit(1);
  }
  for (g_rooms_amount = 0; g_rooms_amount < ROOMS_CAP && leafs.amount; g_rooms_amount++) {
    uint32_t idx = rand() % leafs.amount;
    auto room = &g_rooms[g_rooms_amount];
    room->size = g_rooms_amount == 0 ? V2US(MIN_ROOM_SIZE) : V2U(rand_from_to(MIN_ROOM_SIZE, leafs.data[idx]->section.size.x),
                                                                 rand_from_to(MIN_ROOM_SIZE, leafs.data[idx]->section.size.y));
    room->center = V2U(leafs.data[idx]->section.top_left.x + room->size.x / 2, leafs.data[idx]->section.top_left.y + room->size.y / 2);
    for (uint32_t y = 0; y < room->size.y; y++) {
      for (uint32_t x = 0; x < room->size.x; x++) {
        map_set_point(room->center.x + x - room->size.x/2, room->center.y + y - room->size.y/2, true);
      }
    }
    leafs.data[idx]->room = room;
    leafs.data[idx] = leafs.data[--leafs.amount];
  }
  auto rooms_indices = arena_push_array(arena, true, uint32_t, g_rooms_amount);
  if (!rooms_indices) {
    log_errorlf("%s: ran out of memory", __func__);
    exit(1);
  }
  auto rooms_indices_amount = g_rooms_amount;
  for (uint32_t i = 0; i < rooms_indices_amount; i++) rooms_indices[i] = i;
  for (uint32_t selected_room = 0; rooms_indices_amount > 1;) {
    auto room = &g_rooms[rooms_indices[selected_room]];
    rooms_indices[selected_room] = rooms_indices[--rooms_indices_amount];
    float min_dist = INFINITY;
    for (uint32_t i = 0; i < rooms_indices_amount; i++) {
      auto room_to_check = &g_rooms[rooms_indices[i]];
      auto dist = v2_dist(V2U_V2(room->center), V2U_V2(room_to_check->center));
      if (dist < min_dist) {
        selected_room = i;
        min_dist = dist;
      }
    }
    auto next_room = &g_rooms[rooms_indices[selected_room]];
    carve_corridor(room, next_room);
    if (randf() < 0.25f) {
      auto extra_room = &g_rooms[rand() % g_rooms_amount];
      if (extra_room != next_room && extra_room != room) {
        carve_corridor(room, extra_room);
      }
    }
    room->next = next_room;
  }
}

void
dungeon_gen_update(struct arena *arena) {
  if (window_is_key_press(KEY_INTERACT)) {
    dungeon_gen_init(arena);
  }
}

static struct v2
to_game_coords(uint32_t x, uint32_t y) {
  return V2((x - MAP_W * 0.5f) * UNIT_ONE_PIXEL,
            (y - MAP_H * 0.5f) * UNIT_ONE_PIXEL);
}

void
dungeon_gen_render(void) {
  for (uint32_t y = 0; y < MAP_H; y++) {
    for (uint32_t x = 0; x < MAP_W; x++) {
      if (!map[y*MAP_W+x]) continue;
      renderer_request_point(to_game_coords(x, y), WHITE, 1.0f, 0.0f);
    }
  }
}
