#pragma once
#include "cell.hpp"
#include "consts.hpp"
#include <cstdint>
#include <vector>
#include <unordered_set>

class Soup
{
public:
  Soup();
  bool moveCell(uint16_t id, int16_t &x, int16_t &y);
  int eat(int x, int y);
  bool newCell(int x, int y, int energy, const uint16_t *ram);
  bool isCell(int x, int y);
  int getFoodValue(int x, int y);
  void tick();
  void draw(uint8_t *rgb, int pitch);
private:
  uint16_t food[Height][Width];
  uint16_t cellIds[Height][Width];
  std::vector<Cell> cells, newCells;
  std::unordered_set<uint16_t> freeIds;
};
