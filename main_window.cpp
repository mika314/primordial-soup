#include "main_window.hpp"
#include <GL/gl.h>
#include <iostream>

MainWindow::MainWindow(): Window(Width, Height)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    std::cout << "YUV4MPEG2 W" << Width << " H" << Height << " F" << 30 << ":1 Ip A0:0 C420jpeg XYSCSS=420JPEG\n";
}


void MainWindow::draw()
{
  for (int i = 0; i < 500; ++i)
    soup.tick();
  soup.draw(rgb);
  yuv.resize(Width * Height * 3 / 2);
  auto RGB = std::begin(rgb);
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

  glLoadIdentity();
  glOrtho(0, Width, Height, 0, -1, 1);
  glEnable(GL_TEXTURE_2D);
  glColor3f(1, 1, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb.data());
  glBegin(GL_QUADS);

  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(0, 0);

  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(Width, 0);

  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(Width, Height);

  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(0, Height);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  update();
}
