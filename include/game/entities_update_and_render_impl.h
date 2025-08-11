void
entity_manager_update(float dt) {
  (void)dt;
}

void
entity_manager_render(void) {
  for (uint32_t i = 0; i < entities.cached_amount; i++) {
    auto e = entities.cached[i];
    if (entity_get_flags(e, RENDER_SPRITE)) render_sprite(e);
  }
}
