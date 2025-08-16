void
entity_manager_update(float dt) {
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    e->flags = e->next_flags;
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, FOLLOW_CURSOR)) follow_cursor(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, KEYBOARD_CONTROLLED)) keyboard_control(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, COLLIDABLE)) collide_with_solids(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, MOVABLE)) move(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, CAMERA_FOLLOW)) camera_follow(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, DEPTH_BY_Y)) depth_by_y(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, HOLDING)) move_held(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, HOLDING)) release_held(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, NOT_HOLDING)) hold_released(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, FOLLOW)) follow(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, NOT_HOLDING_GUN)) change_facing(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, HOLDING_GUN)) change_facing_gun(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, MOVABLE|STATE_MACHINE)) update_movable_state(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, FOLLOW_CURSOR|STATE_MACHINE)) update_cursor_state(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, RENDER_ANIMATION|LOOPABLE)) loop_animation(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, RENDER_ANIMATION)) update_animation(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, HAS_GUN|NOT_HOLDING)) activate_gun(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, HAS_GUN|HOLDING)) deactivate_gun(e, dt);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, HOLDING_GUN)) update_gun(e, dt);
  }
}

void
entity_manager_render(void) {
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, RENDER_ANIMATION)) render_animation(e);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, RENDER_SPRITE)) render_sprite(e);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, RENDER_RECT)) render_rect(e);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, RENDER_RADIUS)) render_radius(e);
  }
  for (uint32_t i = 0; i < g_entities.cached_amount; i++) {
    auto e = g_entities.cached[i];
    if (entity_get_flags(e, RENDER_COLLIDER)) render_collider(e);
  }
}
