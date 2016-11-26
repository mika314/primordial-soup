#include "main_window.hpp"
#include <iostream>

MainWindow::MainWindow(bool yuvDump): Window(Width, Height), yuvDump(yuvDump)
{
  if (yuvDump)
  {
    std::cout << "YUV4MPEG2 W" << Width << " H" << Height << " F" << 30 << ":1 Ip A0:0 C420jpeg XYSCSS=420JPEG\n";
  }
}


bool MainWindow::draw(uint8_t *pixels, int pitch)
{
  auto t = SDL_GetTicks() + 1000 / 60;
  while (SDL_GetTicks() < t)
    if (!soup.tick())
      return false;
  soup.draw(pixels, pitch);
  if (yuvDump)
  {
    yuv.resize(Width * Height * 3 / 2);
    auto RGB = pixels;
    auto Y = std::begin(yuv);
    auto U = std::begin(yuv) + Width * Height;
    auto V = std::begin(yuv) + Width * Height + Width / 2 * Height / 2;
    for (auto y = 0; y < Height / 2; ++y)
    {
      for (auto x = 0; x < Width / 2; ++x)
      {
        auto &r0 = *(RGB++);
        auto &g0 = *(RGB++);
        auto &b0 = *(RGB++);
        *Y++ = ((66 * r0 + 129 * g0 + 25 * b0 + 128) >> 8) + 16;
        auto &r1 = *(RGB++);
        auto &g1 = *(RGB++);
        auto &b1 = *(RGB++);
        *Y++ = ((66 * r1 + 129 * g1 + 25 * b1 + 128) >> 8) + 16;
        auto b = (b0 + b1) / 2;
        auto g = (g0 + g1) / 2;
        auto r = (r0 + r1) / 2;
        *U++ = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
        *V++ = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
      }
      for (auto x = 0; x < Width; ++x)
      {
        auto r0 = *(RGB++);
        auto g0 = *(RGB++);
        auto b0 = *(RGB++);
        *Y++ = ((66 * r0 + 129 * g0 + 25 * b0 + 128) >> 8) + 16;
      }
    }
    std::cout << "FRAME\n";
    std::cout.write((const char *)yuv.data(), yuv.size());
  }

  return true;
}
