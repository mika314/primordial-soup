#include "soup.hpp"
#include "asm.hpp"
#include <map>

Soup::Soup()
{
  for (int y = 0; y < Height; ++y)
    for (int x = 0; x < Width; ++x)
    {
      cellIds[y][x] = 0xffff;
      food[y][x] = 0;
    }
}

bool Soup::moveCell(uint16_t id, int16_t &x, int16_t &y)
{
  if (x < 0)
    return false;
  if (x >= Width)
    return false;
  if (y < 0)
    return false;
  if (y >= Height)
    return false;
  if (cellIds[y][x] != 0xffff)
    return false;
  cellIds[cells[id].getY()][cells[id].getX()] = 0xffff;
  cellIds[y][x] = id;
  return true;
}

int Soup::eat(int x, int y)
{
  int res = std::min(static_cast<uint16_t>(100), food[y][x]);
  food[y][x] -= res;
  return res;
}

bool Soup::newCell(int x, int y, int energy, const uint16_t *ram)
{
  if (x < 0)
    return false;
  if (x >= Width)
    return false;
  if (y < 0)
    return false;
  if (y >= Height)
    return false;
  if (energy <= 0)
    return false;
  if (cellIds[y][x] != 0xffff)
    return false;
  uint16_t id;
  if (freeIds.empty())
  {
    id = cells.size() + newCells.size();
    newCells.emplace_back(this, id, x, y, energy, ram);
  }
  else
  {
    id = *freeIds.begin();
    cells[id] = Cell(this, id, x, y, energy, ram);
    freeIds.erase(freeIds.begin());
  }
  cellIds[y][x] = id;
  return true;
}

bool Soup::isCell(int x, int y)
{
  if (x < 0)
    return true;
  if (x >= Width)
    return true;
  if (y < 0)
    return true;
  if (y >= Height)
    return true;
  return cellIds[y][x] != 0xffff;
}

int Soup::getFoodValue(int x, int y)
{
  if (x < 0)
    return 0;
  if (x >= Width)
    return 0;
  if (y < 0)
    return 0;
  if (y >= Height)
    return 0;
  return food[y][x];
}

void Soup::tick()
{
  for (int i = 0; i < Height * Width / 10000; ++i)
  {
    auto x = rand() % Width;
    auto y = rand() % Height;
    auto tmp = rand() % 30;
    if (food[y][x] < 0x7fff - tmp)
      food[y][x] += tmp;
  }
  int c = 0;
  int maxEnergy = 0;
  for (auto &cell: cells)
  {
    if (cell.getEnergy() == 0)
      continue;
    cell.tick();
    if (cell.getEnergy() == 0)
    {
      cellIds[cell.getY()][cell.getX()] = 0xffff;
      freeIds.insert(cell.getId());
    }
    maxEnergy = std::max(cell.getEnergy(), maxEnergy);
    ++c;
  }
  cells.insert(std::end(cells), std::begin(newCells), std::end(newCells));
  c += newCells.size();
  newCells.clear();
  while (c++ < 20)
  {
    enum { ip, tmp, tmp2, energy, food, divide, eat, move, threshold, loop, max, idx, maxI, loop2 };
    newCell(rand() % Width, rand() % Height, 0xffff, Program{
        R[tmp] |= 6, // 0
          R[energy] |= 0x3f,
          R[energy] <<= R[tmp],
          R[energy] |= 0x3f,
          R[energy] <<= R[tmp],
          R[energy] |= 0x3f, // Energy
          R[food] = R[energy],
          R[tmp] ^= R[tmp],
          R[tmp] |= 5,
          R[food] -= R[tmp], // Food
          R[tmp] ^= R[tmp],
          R[tmp] |= 2,
          R[divide] = R[food], // 10
          R[divide] -= R[tmp], // Devide
          R[tmp] ^= R[tmp],
          R[tmp] |= 1,
          R[eat] = R[divide],
          R[eat] -= R[tmp], // Eat
          R[move] = R[eat],
          R[move] -= R[tmp], // Move
          R[threshold] = R[energy],
          R[threshold] >>= R[tmp], 
          R[threshold] >>= R[tmp], // Threshold
          R[loop] = R[ip],
          R[tmp2] = *R[energy],
          R[tmp2] -= R[threshold],
          R[tmp] ^= R[tmp],
          R[tmp] |= 2,
          R[tmp] += R[ip],
          R[tmp2] < R[tmp],
          *R[divide] |= 0,
          R[tmp2] = *R[food],
          R[tmp] ^= R[tmp],
          R[tmp] |= 26,
          R[tmp] += R[ip],
          R[tmp2] != R[tmp], 
          // find max food
          R[max] = *R[food], // max = R[food];
          R[maxI] ^= R[maxI],
          R[idx] ^= R[idx], // for (i = 0; i < 4; ++i)
          R[idx] |= 3,
          R[loop2] = R[ip],
          //   if (R[food + i] > max)
          R[tmp2] = R[food],
          R[tmp2] += R[idx],
          R[tmp] ^= R[tmp],
          R[tmp] |= 1,
          R[tmp2] += R[tmp],
          R[tmp2] = *R[tmp2],
          R[tmp2] -= R[max],
          R[tmp] ^= R[tmp],
          R[tmp] |= 5,
          R[tmp] += R[ip],
          R[tmp2] < R[tmp],
          //   {
          //     max = R[food + i];
          R[tmp2] = R[food],
          R[tmp2] += R[idx],
          R[max] += *R[tmp2],
          //     maxI = i;
          R[maxI] = R[idx],
          //   }
          R[tmp] ^= R[tmp],
          R[tmp] |= 1,
          R[idx] -= R[tmp],
          R[idx] != R[loop2],

          *R[move] = R[maxI],

          *R[eat] |= 0,
          R[ip] = R[loop]
        }.data());
  }
}

void Soup::draw(uint8_t *rgb, int pitch)
{
  std::map<uint16_t, int> hist;
  for (int y = 0; y < Height; ++y)
    for (int x = 0; x < Width; ++x)
      ++hist[food[y][x]];
  auto sum = 0;
  auto iter = hist.rbegin();
  for (; iter != hist.rend(); ++iter)
  {
    sum += iter->second;
    if (sum > 5000)
      break;
  }
  auto KF = (iter != hist.rend()) ? iter->first : 10;
  for (int y = 0; y < Height; ++y)
    for (int x = 0; x < Width; ++x)
    {
      rgb[y * pitch + x * 3] = 0;
      rgb[y * pitch + x * 3 + 1] = std::min(255, food[y][x] * 128 / KF);
      rgb[y * pitch + x * 3 + 2] = std::min(255, food[y][x] * 255 / KF);
    }
  const auto KE = 20000;
  for (auto &cell: cells)
  {
    if (cell.getEnergy() == 0)
      continue;
    auto y = cell.getY();
    auto x = cell.getX();
    rgb[y * pitch + x * 3] = std::max(0, std::min(255, (KE - cell.getEnergy()) * 0x7f / KE));
    rgb[y * pitch + x * 3 + 1] = std::min(255, cell.getEnergy() * 0xff / KE);
    rgb[y * pitch + x * 3 + 2] = 0;
  }
}
