#pragma once
#include "cell.hpp"
#include <cstdint>
#include <initializer_list>

struct OpCode
{
  uint16_t data;
};

struct RegPtr;

#define OPS                                     \
  X(=, Mov)                                     \
  X(+=, Add)                                    \
  X(-=, Sub)                                    \
  X(*=, Mul)                                    \
  X(/=, Div)                                    \
  X(&=, And)                                    \
  X(|=, Or)                                     \
  X(^=, Xor)                                    \
  X(<<=, ShiftLeft)                             \
  X(>>=, ShiftRight)                            \
  X(==, Jeq)                                    \
  X(!=, Jne)                                    \
  X(>, Jgt)                                     \
  X(<, Jlt)

struct Reg
{
  uint16_t data;
  constexpr OpCode operator|=(unsigned) const;
#define X(op, opCode)   constexpr OpCode operator op(const RegPtr &y) const; \
  constexpr OpCode operator op(const Reg &y) const;
OPS
#undef X
};

struct RegPtr
{
  uint16_t data;
  constexpr Reg operator*() const;
  constexpr OpCode operator|=(unsigned) const;

#define X(op, opCode)   constexpr OpCode operator op(const RegPtr &y) const; \
  constexpr OpCode operator op(const Reg &y) const;
OPS
#undef X
};

constexpr OpCode Reg::operator|=(unsigned y) const
{
  return { static_cast<uint16_t>(Cell::Load | (data << 4) | (1 << 9) | (y << 10)) };
}

constexpr Reg RegPtr::operator*() const
{
  return Reg{ data };
}

#define X(op, opCode)                                                   \
  constexpr OpCode Reg::operator op(const RegPtr &y) const              \
  {                                                                     \
    return { static_cast<uint16_t>(Cell::opCode | (data << 4) | (1 << 9) | (y.data << 10)) }; \
  }                                                                     \
  constexpr OpCode Reg::operator op(const Reg &y) const                 \
  {                                                                     \
    return { static_cast<uint16_t>(Cell::opCode | (data << 4) | (1 << 9) | (y.data << 10) | (1 << 15)) }; \
  }
OPS
#undef X

constexpr OpCode RegPtr::operator|=(unsigned y) const
{
  return { static_cast<uint16_t>(Cell::Load | (data << 4) | (y << 10)) };
}

#define X(op, opCode)                                                   \
  constexpr OpCode RegPtr::operator op(const RegPtr &y) const           \
  {                                                                     \
    return { static_cast<uint16_t>(Cell::opCode | (data << 4) | (y.data << 10)) }; \
  }                                                                     \
  constexpr OpCode RegPtr::operator op(const Reg &y) const              \
  {                                                                     \
    return { static_cast<uint16_t>(Cell::opCode | (data << 4) | (y.data << 10) | (1 << 15)) }; \
  }
OPS
#undef X



const RegPtr R[32] = {
  0, 1, 2, 3, 4, 5, 6, 7,
  8, 9, 10, 11, 12, 13, 14, 15,
  16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27, 28, 29, 30, 31
};

class Program
{
public:
  Program(std::initializer_list<OpCode> l);
  const uint16_t *data() const;
private:
  uint16_t ram[1024];
};
