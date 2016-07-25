#include "window.hpp"
#include <stdexcept>
#include <SDL2/SDL.h>
#include <GL/gl.h>

class PrivateWindow
{
public:
    SDL_Renderer *renderer;
    SDL_Window *window;
};

const int Window::KLeft = SDLK_LEFT;
const int Window::KRight = SDLK_RIGHT;
const int Window::KUp = SDLK_UP;
const int Window::KDown = SDLK_DOWN;
const int Window::KSpace = SDLK_SPACE;

Window::Window(int width, int height):
    width_(width),
    height_(height),
    needUpdate_(true),
    lastUpdate_(0),
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

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);   
}

Window::~Window()
{
    SDL_DestroyRenderer(private_->renderer);
    SDL_DestroyWindow(private_->window);
    delete private_;
    SDL_Quit();
}

void Window::update()
{
    SDL_Event event;
    event.type = SDL_WINDOWEVENT;
    event.window.event = SDL_WINDOWEVENT_EXPOSED;
    SDL_PeepEvents(&event, 1, SDL_ADDEVENT, 0, 0);
}

void Window::draw()
{
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
        if (SDL_WaitEvent(&e))
        {
            switch (e.type)
            {
            case SDL_WINDOWEVENT:
                if (e.window.event == SDL_WINDOWEVENT_EXPOSED)
                    needUpdate_ = true;
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
        const auto isEmpty = (SDL_PeepEvents(&e, 1, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) == 0);
        if (isEmpty || SDL_GetTicks() > lastUpdate_ + 1000 / 60)
        {
            if (needUpdate_)
            {
                glClear(GL_COLOR_BUFFER_BIT);
                draw();
                SDL_RenderPresent(private_->renderer);
                needUpdate_ = false;
            }
            lastUpdate_ = SDL_GetTicks();
        }
    }
    return 0;
}
