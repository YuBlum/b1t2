void
entity_manager_update(float dt) {
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    e->flags = e->next_flags;
  }
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, NOT_MOVING|KEYBOARD_CONTROLLED)) keyboard_control(e, dt);
  }
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, MOVING)) move(e, dt);
  }
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, HOLDING)) move_held(e, dt);
  }
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, HOLDING)) release_held(e, dt);
  }
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, NOT_HOLDING)) hold_released(e, dt);
  }
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, FOLLOW)) follow(e, dt);
  }
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, MOVING|FACING)) change_facing(e, dt);
  }
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, STATE_MACHINE)) update_state(e, dt);
  }
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, STATE_MACHINE)) update_animation(e, dt);
  }
}

void
entity_manager_render(void) {
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, STATE_MACHINE)) render_state_animation(e);
  }
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, RENDER_SPRITE)) render_sprite(e);
  }
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, RENDER_RECT)) render_rect(e);
  }
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, RENDER_RADIUS)) render_radius(e);
  }
}
