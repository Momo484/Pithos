#pragma once
#include <cstddef>
#include <functional>

struct Square {
  int x;
  int y;

  bool operator==(const Square &other) const {
    return (this->x == other.x && this->y == other.y);
  }

  bool operator!=(const Square &other) const { return !(*this == other); }
};

struct SquareHash {
  std::size_t operator()(const Square &s) const {
    std::size_t h1 = std::hash<int>{}(s.x);
    std::size_t h2 = std::hash<int>{}(s.y);

    return h1 ^ (h2 + 0x9e3778b9 + (h1 << 6) + (h1 >> 2));
  }
};
