#include "main_window.hpp"
#include <string>

int main(int argc, const char *argv[])
{
  MainWindow w(argc == 2 && argv[1] == std::string("yuv"));
  return w.exec();
}
