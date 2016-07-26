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
    MoveAddr       = 0xfff6, // writing to this address makes cell to move 0 - left, 1 - right, 2 - top, 3 - bottom 
    EatAddr        = 0xfff7, // writing to this address makes cell to eat
    DivideAddr     = 0xfff8, // writing to the memory with this addres will make the cell divide 0 - left, 1 - right, 2 - top, 3 - bottom
    IsCellAddr     = 0xfff9, // reading from memory with this addres will return bit mask with information about cells arount 0 bit - left, 1st bit - right, 2nd bit - top, 3rd bit - bottom
    UnderFoodAddr  = 0xfffa, // reading from memory with this addres will return amount of food UNDER the cell
    LeftFoodAddr   = 0xfffb, // reading from memory with this addres will return amount of food on LEFT (WEST) side of the cell
    RightFoodAddr  = 0xfffc, // reading from memory with this addres will return amount of food on RIGHT (EAST) side of the cell
    TopFoodAddr    = 0xfffd, // reading from memory with this addres will return amount of food on TOP (NORTH) side of the cell
    BottomFoodAddr = 0xfffe, // reading from memory with this addres will return amount of food on BOTTOM (SOUTH) side of the cell
    EnergyAddr     = 0xffff, // reading from memory with this addres will return amount of energy the cell has
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
*/
