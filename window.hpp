#pragma once

class PrivateWindow;

class Window
{
public:
    static const int KLeft;
    static const int KRight;
    static const int KUp;
    static const int KDown;
    static const int KSpace;
    Window(int width = 800, int height = 600);
    ~Window();
    int exec();
    void update();
protected:
    virtual void draw();
    virtual void keyPressEvent(int key);
    virtual void keyReleaseEvent(int key);
private:
    int width_;
    int height_;
    bool needUpdate_;
    unsigned lastUpdate_;
    PrivateWindow *private_;
};
