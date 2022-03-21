#include "cell.hpp"
#include "soup.hpp"
#include <iostream>
#include <sstream>

Cell::Cell(Soup *soup, uint16_t id, int x, int y, unsigned energy, const uint16_t *aram):
  id(id),
  soup(soup),
  x(x),
  y(y),
  energy(energy),
  reg{}
{
  for (int i = 0; i < RamSize; ++i)
    ram[i] = aram[i];
}

std::string Cell::opCodeToString(uint16_t opCode)
{
  std::ostringstream res;
  if (opCode % 16 != Nop)
    res << ((opCode & 0x0200) != 0 ? "*" : "")
        << "R[" << ((opCode >> 4) & 0x1f) << "] ";
  switch (opCode % 16)
  {
  case Nop:
    res << "nop";
    break;
  case Mov:
    res << "=";
    break;
  case Load:
    res << "|=";
    break;
  case Add:
    res << "+=";
    break;
  case Sub:
    res << "-=";
    break;
  case Mul:
    res << "*=";
    break;
  case Div:
    res << "/=";
    break;
  case And:
    res << "&=";
    break;
  case Or:
    res << "|=";
    break;
  case Xor:
    res << "^=";
    break;
  case ShiftLeft:
    res << "<<=";
    break;
  case ShiftRight:
    res << ">>=";
    break;
  case Jeq:
    res << "==";
    break;
  case Jne:
    res << "!=";
    break;
  case Jgt:
    res << ">";
    break;
  case Jlt:
    res << "<";
    break;
  }
  if (opCode % 16 != Nop && opCode % 16 != Load)
    res << " " << ((opCode & 0x8000) != 0 ? "*" : "")
        << "R[" << ((opCode >> 10) & 0x1f) << "] ";
  else if (opCode % 16 == Load)
    res << " " << ((opCode >> 10) & 0x3f);
  return res.str();
 }

void Cell::tick()
{
  if (rand() % 40000 == 0)
    setRam(rand() % RamSize, rand() % 0x1000); // make cell mutate
  auto opCode = getRam(reg[0]++); // reg[0] is IP (Instruction Pointer) register
  if (id == 0 && false)
  {
    std::clog << id << " " << opCodeToString(opCode);
    int i = 0;
    for (auto r: reg)
      std::clog << " R[" << i++ << "]=" << r;
    std::clog << std::endl;
  }
  auto isLeftIndirect = (opCode & 0x0200) != 0;
  auto &leftReg = reg[(opCode >> 4) & 0x1f];
  auto value = reg[(opCode >> 10) & 0x1f];
  if ((opCode & 0x8000) != 0)
    value = getRam(value);
  switch (opCode % 16)
  {
  case Nop:
    break;
  case Mov:
    if (isLeftIndirect)
      setRam(leftReg, value);
    else
      leftReg = value;
    break;
  case Load:
    if (isLeftIndirect)
      setRam(leftReg, getRam(leftReg) | ((opCode >> 10) & 0x3f));
    else
      leftReg = leftReg | ((opCode >> 10) & 0x3f);
    break;
  case Add:
    if (isLeftIndirect)
      setRam(leftReg, getRam(leftReg) + value);
    else
      leftReg += value;
    break;
  case Sub:
    if (isLeftIndirect)
      setRam(leftReg, getRam(leftReg) - value);
    else
      leftReg -= value;
    break;
  case Mul:
    if (isLeftIndirect)
      setRam(leftReg, getRam(leftReg) * value);
    else
      leftReg *= value;
    break;
  case Div:
    if (value != 0)
    {
      if (isLeftIndirect)
        setRam(leftReg, getRam(leftReg) / value);
      else
        leftReg /= value;
    }
    else
    {
      if (isLeftIndirect)
        setRam(leftReg, 0x7fff);
      else
        leftReg = 0x7fff;
    }
    break;
  case And:
    if (isLeftIndirect)
      setRam(leftReg, getRam(leftReg) & value);
    else
      leftReg &= value;
    break;
  case Or:
    if (isLeftIndirect)
      setRam(leftReg, getRam(leftReg) | value);
    else
      leftReg |= value;
    break;
  case Xor:
    if (isLeftIndirect)
      setRam(leftReg, getRam(leftReg) ^ value);
    else
      leftReg ^= value;
    break;
  case ShiftLeft:
    if (isLeftIndirect)
      setRam(leftReg, getRam(leftReg) << value);
    else
      leftReg <<= value;
    break;
  case ShiftRight:
    if (isLeftIndirect)
      setRam(leftReg, getRam(leftReg) >> value);
    else
      leftReg >>= value;
    break;
  case Jeq:
    if (isLeftIndirect)
    {
      if (getRam(leftReg) == 0)
        reg[0] = value;
    }
    else
    {
      if (leftReg == 0)
        reg[0] = value;
    }
    break;
  case Jne:
    if (isLeftIndirect)
    {
      if (getRam(leftReg) != 0)
        reg[0] = value;
    }
    else
    {
      if (leftReg != 0)
        reg[0] = value;
    }
    break;
  case Jgt:
    if (isLeftIndirect)
    {
      if (getRam(leftReg) != 0 && (getRam(leftReg) & 0x8000) == 0)
        reg[0] = value;
    }
    else
    {
      if (leftReg != 0 && (leftReg & 0x8000) == 0)
        reg[0] = value;
    }
    break;
  case Jlt:
    if (isLeftIndirect)
    {
      if (getRam(leftReg) != 0 && (getRam(leftReg) & 0x8000) != 0)
        reg[0] = value;
    }
    else
    {
      if (leftReg != 0 && (leftReg & 0x8000) != 0)
        reg[0] = value;
    }
    break;
  }
  if (energy > 0)
    --energy;
  ++age;
}

uint16_t Cell::getRam(uint16_t addr)
{
  if (addr < RamSize)
    return ram[addr];
  switch (addr)
  {
  case IsCellAddr:
    return soup->isCell(x - 1, y) &
      (soup->isCell(x + 1, y) << 1) &
      (soup->isCell(x, y - 1) << 1) &
      (soup->isCell(x, y + 1) << 1);
  case UnderFoodAddr:
    return soup->getFoodValue(x, y);
  case LeftFoodAddr:
    return soup->getFoodValue(x - 1, y);
  case RightFoodAddr:
    return soup->getFoodValue(x + 1, y);
  case TopFoodAddr:
    return soup->getFoodValue(x, y - 1);
  case BottomFoodAddr:
    return soup->getFoodValue(x, y + 1);
  case EnergyAddr:
    return energy;
  }
  return 0;
}

void Cell::setRam(uint16_t addr, uint16_t value)
{
  if (addr < RamSize)
  {
    ram[addr] = value;
    return;
  }
  switch (addr)
  {
  case MoveAddr:
    if (energy == 0)
      break;
    {
      auto newX = x;
      auto newY = y;
      switch (value % 4)
      {
      case 0:
        --newX;
        break;
      case 1:
        ++newX;
        break;
      case 2:
        --newY;
        break;
      case 3:
        ++newY;
        break;
      }
      if (soup->moveCell(id, newX, newY))
      {
        x = newX;
        y = newY;
      }
      --energy;
    }
    break;
  case EatAddr:
    {
      auto tmp = soup->eat(x, y);
      if (tmp + energy < 0x7fff)
        energy += tmp;
      else
        energy = 0x7fff;
    }
    break;
  case DivideAddr:
    {
      int newX = x;
      int newY = y;
      switch (value % 4)
      {
      case 0:
        --newX;
        break;
      case 1:
        ++newX;
        break;
      case 2:
        --newY;
        break;
      case 3:
        ++newY;
        break;
      }
      if (soup->newCell(newX, newY, energy / 2, getRam()))
        energy -= energy / 2;
    }
    break;
  }
}

int Cell::getEnergy() const
{
  return energy;
}

int Cell::getX() const
{
  return x;
}
int Cell::getY() const
{
  return y;
}

uint16_t Cell::getId() const
{
  return id;
}

const uint16_t *Cell::getRam() const
{
  return ram;
}
