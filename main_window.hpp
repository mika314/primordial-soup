#pragma once
#include "consts.hpp"
#include "soup.hpp"
#include "window.hpp"
#include <SDL.h>
#include <vector>

class MainWindow: public Window
{
public:
  MainWindow(bool yuvDump = false);
  bool draw(uint8_t *pixels, int pitch) override;
private:
  Soup soup;
  std::vector<uint8_t> yuv;
  bool yuvDump;
};
