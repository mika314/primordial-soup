#include "asm.hpp"
#include <cstdlib>

struct RegPtr;

Program::Program(std::initializer_list<OpCode> l)
{
  for (int j = 0; j < 1024; ++j)
    ram[j] = rand() % 0x10000;
  auto i = ram;
  for (const auto &j: l)
    *i++ = j.data;
}

const uint16_t *Program::data() const
{
  return ram;
}
