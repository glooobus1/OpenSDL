#pragma once
#include "SimpleRenderOpenSDL.h"
#include <vector>
#include <map>
#include <cstdint>
#include <string> 
#include <ctime>    
#include <cstdlib>

class Spdl {
private:
    int windowWidth = 800;
    int windowHeight = 600;
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    bool running = true;

public:
    Spdl();
    ~Spdl();

    void initialization();
    void Render();
    void handleEvents();
    void update(float deltaTime);
    bool isRunning() const { return running; }
    SDL_Window* getWindow() const { return window; }
    int getWidth() const { return windowWidth; }
    int getHeight() const { return windowHeight; }

    // Публичные функции рисования (их можно вызывать из main)
    void DrawLine2d(int x1, int y1, int x2, int y2, Color color);
    void DrawRect2d(int x, int y, int w, int h, Color color);
    void DrawCircle2d(int cx, int cy, int radius, Color color);

    void DrawFillRect2d(int x, int y, int w, int h, Color color);
    void DrawFillCircle2d(int cx, int cy, int radius, Color color);
    void DrawFillTriangle2d(int x1, int y1, int x2, int y2, int x3, int y3, Color color);

    // Очистка экрана (для main)
    void Clear(Color color = Color::black());
    void SwapBuffers() { 
        if (window) SDL_GL_SwapWindow(window);
    }
};