#include "game/global.h"

struct global global;

void
global_init(void) {
  static_assert(
    sizeof (struct global) ==
      sizeof (int),
    "added global variable but didn't set it up"
  );
}

