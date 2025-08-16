#include "engine/renderer.h"
#include "engine/window.h"
#include <string.h>

#define MIN_SECTION_SIZE 10
#define MIN_ROOM_SIZE 6
#define ROOMS_CAP 10
#define MAP_OFFSET 6
#define MAP_W (50+MAP_OFFSET*2)
#define MAP_H (50+MAP_OFFSET*2)

enum split_direction {
  SPLIT_HORIZONTAL = 0,
  SPLIT_VERTICAL,
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

struct sections {
  struct section *data;
  uint32_t amount;
  uint32_t capacity;
};

struct bsp_list {
  struct bsp_list *next;
  struct bsp_node *node;
};

static struct room g_rooms[ROOMS_CAP];
static uint32_t    g_rooms_amount;
static bool g_map[MAP_W*MAP_H];
struct v2u g_came_from[ROOMS_CAP][MAP_H][MAP_W];

static inline struct v2u room_get_top(uint32_t i)    { return V2U(g_rooms[i].center.x, g_rooms[i].center.y + g_rooms[i].size.y/2); }
static inline struct v2u room_get_left(uint32_t i)   { return V2U(g_rooms[i].center.x - g_rooms[i].size.x/2, g_rooms[i].center.y); }
static inline struct v2u room_get_bottom(uint32_t i) { return V2U(g_rooms[i].center.x, g_rooms[i].center.y - g_rooms[i].size.y/2); }
static inline struct v2u room_get_right(uint32_t i)  { return V2U(g_rooms[i].center.x + g_rooms[i].size.x/2, g_rooms[i].center.y); }

static void
map_set_point(bool *map, uint32_t x, uint32_t y, bool value) {
  if (x >= MAP_W || y >= MAP_H) return;
  map[y*MAP_W+x] = value;
}

static bool
map_get_point(bool *map, uint32_t x, uint32_t y) {
  if (x >= MAP_W || y >= MAP_H) return false;
  return map[y*MAP_W+x];
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
bsp_get_sections(struct bsp_node *node, struct sections *sections) {
  bool is_leaf = true;
  if (node->children[0]) {
    bsp_get_sections(node->children[0], sections);
    is_leaf = false;
  }
  if (node->children[1]) {
    bsp_get_sections(node->children[1], sections);
    is_leaf = false;
  }
  if (!is_leaf) return;
  if (sections->amount >= sections->capacity) {
    log_errorlf("%s: unreachable", __func__);
    exit(1);
  }
  sections->data[sections->amount++] = node->section;
}

static void
carve_corridor(struct room *a, struct room *b) {
  uint32_t x0 = a->center.x, y0 = a->center.y,
           x1 = b->center.x, y1 = b->center.y;
  if (rand() % 2) {
    for (uint32_t x = MIN(x0,x1); x <= MAX(x0,x1); x++) map_set_point(g_map, x, y0, true);
    for (uint32_t y = MIN(y0,y1); y <= MAX(y0,y1); y++) map_set_point(g_map, x1, y, true);
  } else {
    for (uint32_t y = MIN(y0,y1); y <= MAX(y0,y1); y++) map_set_point(g_map, x0, y, true);
    for (uint32_t x = MIN(x0,x1); x <= MAX(x0,x1); x++) map_set_point(g_map, x, y1, true);
  }
}

void
dungeon_gen_init(struct arena *arena) {
  memset(g_map, 0, sizeof (g_map));
  struct bsp_node root = {
    .section = {
      .top_left = { MAP_OFFSET, MAP_OFFSET },
      .size     = { MAP_W-MAP_OFFSET*2, MAP_H-MAP_OFFSET*2 }
    }
  };
  struct sections sections;
  sections.capacity = bsp_make_sections(arena, &root, randf() < 0.5f ? SPLIT_VERTICAL : SPLIT_HORIZONTAL, 0);
  sections.data     = arena_push_array(arena, true, struct section, sections.capacity);
  sections.amount   = 0;
  if (!sections.data) {
    log_errorlf("%s: ran out of memory", __func__);
    exit(1);
  }
  bsp_get_sections(&root, &sections);
  if (sections.capacity != sections.amount) {
    log_errorlf("%s: unreachable", __func__);
    exit(1);
  }
  for (g_rooms_amount = 0; g_rooms_amount < ROOMS_CAP && sections.amount; g_rooms_amount++) {
    uint32_t idx = rand() % sections.amount;
    auto room = &g_rooms[g_rooms_amount];
    room->size = g_rooms_amount == 0 ? V2US(MIN_ROOM_SIZE) : V2U(rand_from_to(MIN_ROOM_SIZE, sections.data[idx].size.x),
                                                                 rand_from_to(MIN_ROOM_SIZE, sections.data[idx].size.y));
    room->center = V2U(sections.data[idx].top_left.x + room->size.x / 2, sections.data[idx].top_left.y + room->size.y / 2);
    for (uint32_t y = 0; y < room->size.y; y++) {
      for (uint32_t x = 0; x < room->size.x; x++) {
        map_set_point(g_map, room->center.x + x - room->size.x/2, room->center.y + y - room->size.y/2, true);
      }
    }
    //uint32_t holes = room->size.x*room->size.y >= 50 ? rand_from_to(10, 20) : rand()%10;
    //for (uint32_t i = 0; i < holes; i++) {
    //  map_set_point(g_map, sections.data[idx].top_left.x + rand() % room->size.x, sections.data[idx].top_left.y + rand() % room->size.y, false);
    //}
    sections.data[idx] = sections.data[--sections.amount];
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
    if (rand() % 2 == 0) {
      struct room *extra_room[2] = { 0 };
      uint32_t tries = 0;
      do {
        extra_room[0] = &g_rooms[rand() % g_rooms_amount];
        tries++;
      } while((extra_room[0] == next_room || extra_room[0] == room) && tries < 10);
      tries = 0;
      do {
        extra_room[1] = &g_rooms[rand() % g_rooms_amount];
        tries++;
      } while((extra_room[1] == next_room || extra_room[1] == room || extra_room[1] == extra_room[0]) && tries < 10);
      if (extra_room[0] && !extra_room[1]) {
        carve_corridor(room, extra_room[0]);
      } else if (!extra_room[0] && extra_room[1]) {
        carve_corridor(room, extra_room[1]);
      } else if (extra_room[0] && extra_room[1]) {
        uint32_t e = v2u_dist(room->center, extra_room[0]->center) < v2u_dist(room->center, extra_room[1]->center) ? 0 : 1;
        carve_corridor(room, extra_room[e]);
      }
    }
    room->next = next_room;
  }
  uint32_t noise = rand_from_to(200, 400);
  for (uint32_t i = 0; i < noise; i++) {
    uint32_t nx = MAP_W, ny = MAP_H;
    uint32_t x, y;
    do {
      if (nx >= MAP_W || ny >= MAP_H) {
        nx = rand() % MAP_W;
        ny = rand() % MAP_H;
      }
      x = nx; y = ny;
      switch (rand() % 4) {
        case 0: nx = (x + 1); ny = (y + 0); break;
        case 1: nx = (x - 1); ny = (y + 0); break;
        case 2: nx = (x + 0); ny = (y + 1); break;
        case 3: nx = (x + 0); ny = (y - 1); break;
      }
    } while (!map_get_point(g_map, nx, ny));
    map_set_point(g_map, x, y, true);
  }

  bool map_visited[MAP_W*MAP_H];
  struct v2u queue[MAP_W*MAP_H];
  int32_t dirs_x[4] = { 1, -1, 0,  0 };
  int32_t dirs_y[4] = { 0,  0, 1, -1 };
  auto start = g_rooms[0].center;
  for (uint32_t i = 1; i < g_rooms_amount; i++) {
    uint32_t queue_front = 0, queue_back = 0;
    auto target = g_rooms[i].center;
    memset(map_visited, 0, sizeof (map_visited));
    map_set_point(map_visited, start.x, start.y, true);
    queue[queue_back++] = start;
    bool found_path = false;
    struct v2u cur;
    while (queue_front < queue_back) {
      cur = queue[queue_front++];
      if (cur.x == target.x && cur.y == target.y) {
        found_path = true;
        break;
      }
      for (uint32_t j = 0; j < 4; j++) {
        auto p = V2U(cur.x + dirs_x[j], cur.y + dirs_y[j]);
        if (p.x > MAP_W-1 || p.y > MAP_H-1 || map_get_point(map_visited, p.x, p.y) || !map_get_point(g_map, p.x, p.y)) continue;
        map_set_point(map_visited, p.x, p.y, true);
        g_came_from[i][p.y][p.x] = cur;
        queue[queue_back++] = p;
      }
    }
    if (found_path) {
      uint32_t steps = 1;
      for (; cur.x != start.x || cur.y != start.y; cur = g_came_from[i][cur.y][cur.x]) steps++;
      log_infolf("found with %u steps", steps);
    } else {
      log_infol("not found");
    }
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
      renderer_request_point(to_game_coords(x, y), map_get_point(g_map, x, y) ? WHITE : RED, 1.0f, 0.0f);
    }
  }
  for (uint32_t i = 1; i < g_rooms_amount; i++) {
    for (struct v2u cur = g_rooms[i].center; cur.x != g_rooms[0].center.x || cur.y != g_rooms[0].center.y; cur = g_came_from[i][cur.y][cur.x]) {
      renderer_request_point(to_game_coords(cur.x, cur.y), BLACK, 0.2f, 0.0f);
    }
  }
  for (uint32_t i = 0; i < g_rooms_amount; i++) {
    renderer_request_point(to_game_coords(g_rooms[i].center.x, g_rooms[i].center.y), i == 0 ? BLUE : RGB(0.0f, 0.8f, 0.0f), 1.0f, 0.0f);
  }
}
