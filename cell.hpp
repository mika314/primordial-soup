#pragma once
#include <cstdint>
#include <string>

class Soup;

class Cell
{
public:
  Cell(Soup *, uint16_t id, int x, int y, unsigned energy, const uint16_t *ram);
  const uint16_t *getRam() const;
  void tick();
  uint16_t getId() const;
  int getEnergy() const;
  int getX() const;
  int getY() const;
  enum {
    Nop,
    Mov,
    Load,
    Add,
    Sub,
    Mul,
    Div,
    And,
    Or,
    Xor,
    ShiftLeft,
    ShiftRight,
    Jeq,
    Jne,
    Jgt,
    Jlt,
  };
  enum {
    MoveAddr = 0xfff6,
    EatAddr = 0xfff7,
    DivideAddr = 0xfff8,
    IsCellAddr = 0xfff9,
    UnderFoodAddr = 0xfffa,
    LeftFoodAddr = 0xfffb,
    RightFoodAddr = 0xfffc,
    TopFoodAddr = 0xfffd,
    BottomFoodAddr = 0xfffe,
    EnergyAddr = 0xffff
  };
private:
  uint16_t id;
  Soup *soup;
  int16_t x;
  int16_t y;
  unsigned age = 0;
  uint16_t energy;
  uint16_t reg[32]; // reg[0] - ip
  enum { RamSize = 1024 };
  uint16_t ram[RamSize];
  uint16_t getRam(uint16_t addr);
  void setRam(uint16_t addr, uint16_t value);
  static std::string opCodeToString(uint16_t opCode);
};

/*
  op code
  [][][][][][][][][][][][][][][][]
  [      ]-command
          []-is indirect
            [        ]-
                       0 ip register
                       1-31 register
                      []-is indirect
                        [        ]-
                                   0 ip register
                                   1-31 register

 0xfff6 - move writing to this address makes cell to move 0 - left, 1 - right, 2 - top, 3 - bottom 
 0xfff7 - eat writing to this address makes cell to eat 10 
 0xfff8 - divide 0 - left, 1 - right, 2 - top, 3 - bottom
 0xfff9 - is cell 0 bit - left, 1st bit - right, 2nd bit - top, 3rd bit - bottom
 0xfffa - food under
 0xfffb - food on left
 0xfffc - food on right
 0xfffd - food on top
 0xfffe - food on bottom
 0xffff - energy
 */
