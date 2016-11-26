#include "window.hpp"
#include <SDL.h>
#include <stdexcept>
#include <string>

class PrivateWindow
{
public:
  SDL_Renderer *renderer;
  SDL_Window *window;
  SDL_Texture *texture;
};

const int Window::KLeft = SDLK_LEFT;
const int Window::KRight = SDLK_RIGHT;
const int Window::KUp = SDLK_UP;
const int Window::KDown = SDLK_DOWN;
const int Window::KSpace = SDLK_SPACE;

Window::Window(int width, int height):
  width_(width),
  height_(height),
  private_(new PrivateWindow)
{
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    throw std::runtime_error("SDL_Init Error: " + std::string(SDL_GetError()));
  SDL_CreateWindowAndRenderer(width_, height_, SDL_WINDOW_OPENGL, &private_->window, &private_->renderer);
  SDL_RendererInfo rendererInfo;
  SDL_GetRendererInfo(private_->renderer, &rendererInfo);
  if ((rendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 || 
      (rendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) 
    throw std::runtime_error("SDL_GetRendererInfo Error: " + std::string(SDL_GetError()));

  private_->texture = SDL_CreateTexture(private_->renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, width_, height_);
  if (!private_->texture)
    throw std::runtime_error("SDL_CreateTexture Error: " + std::string(SDL_GetError()));
}

Window::~Window()
{
  SDL_DestroyRenderer(private_->renderer);
  SDL_DestroyWindow(private_->window);
  SDL_DestroyTexture(private_->texture);
  delete private_;
  SDL_Quit();
}

bool Window::draw(uint8_t *pixels, int pitch)
{
  return true;
}

void Window::keyPressEvent(int)
{
}

void Window::keyReleaseEvent(int)
{
}


int Window::exec()
{
  bool done = false;
  while (!done)
  {
    SDL_Event e;
    if (SDL_PollEvent(&e))
    {
      switch (e.type)
      {
      case SDL_WINDOWEVENT:
        if (e.window.event == SDL_WINDOWEVENT_EXPOSED)
          break;
      case SDL_KEYDOWN:
        keyPressEvent(e.key.keysym.sym);
        break;
      case SDL_KEYUP:
        keyReleaseEvent(e.key.keysym.sym);
        break;
      case SDL_QUIT:
        done = true;
        break;
      }
    }
    uint8_t *pixels;
    int pitch;
    if (SDL_LockTexture(private_->texture,
                        nullptr,
                        (void **)&pixels,
                        &pitch) != 0)
      throw std::runtime_error("SDL_LockTexture Error: " + std::string(SDL_GetError()));

    if (!draw(pixels, pitch))
      done = true;

    SDL_UnlockTexture(private_->texture);

    if (SDL_RenderCopy(private_->renderer, private_->texture, nullptr, nullptr) != 0)
      throw std::runtime_error("SDL_RenderCopy Error: " + std::string(SDL_GetError()));

    SDL_RenderPresent(private_->renderer);
  }
  return 0;
}
