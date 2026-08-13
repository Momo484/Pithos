#pragma once

#include "types/GameState.hpp"
#include "types/Move.hpp"

struct BoardMemento {
  Move move;
  GameState state;
};
