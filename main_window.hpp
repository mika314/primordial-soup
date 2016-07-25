#pragma once
#include "soup.hpp"
#include "consts.hpp"
#include <a/window.hpp>
#include <vector>

class MainWindow: public Window
{
public:
  MainWindow();
  void draw() override;
private:
  Soup soup;
  std::vector<uint8_t> rgb;
  std::vector<uint8_t> yuv;
};
