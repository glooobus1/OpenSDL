#include <cstdio>
#include <cmath>
#include <iostream>
#include "SimpleSDL.hpp"
#include "DebagLogSystem.hpp"

// ===== КОНСТРУКТОР =====
Spdl::Spdl() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL Error: " << SDL_GetError() << std::endl;
        running = false;
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow(
        "Spdl Engine",
        windowWidth, windowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cout << "Window Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        running = false;
        return;
    }

    glContext = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Отключаем антиалиасинг для линий
    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POLYGON_SMOOTH);

    // Убираем белые артефакты
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Устанавливаем точное совмещение пикселей
    glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
    
    // дебаг
    INC_Spdl();
    std::cout << "Spdl Engine !" << std::endl;
}

// ===== ДЕСТРУКТОР =====
Spdl::~Spdl() {
    if (glContext) SDL_GL_DestroyContext(glContext);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

// ===== ОЧИСТКА ЭКРАНА =====
void Spdl::Clear(Color color) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// ===== RENDER - ТОЛЬКО НАСТРАИВАЕТ ПРОЕКЦИЮ =====
void Spdl::Render() {
    // Настройка проекции под текущие размеры
    glViewport(0, 0, windowWidth, windowHeight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1); // 2D режим

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Смена буферов происходит ПОСЛЕ того, как main нарисует
}

// ===== ФУНКЦИИ РИСОВАНИЯ =====
void Spdl::DrawLine2d(int x1, int y1, int x2, int y2, Color color) {
    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_LINES);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glEnd();
}

void Spdl::DrawRect2d(int x, int y, int w, int h, Color color) {
    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);
    glEnd();
}

void Spdl::DrawCircle2d(int cx, int cy, int radius, Color color) {
    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 32; i++) {
        float angle = 2.0f * 3.1415f * i / 32;
        glVertex2f(cx + cos(angle) * radius, cy + sin(angle) * radius);
    }
    glEnd();
}

// ===== ОСТАЛЬНЫЕ МЕТОДЫ =====
void Spdl::initialization() {
    INC_Spdl();
}

void Spdl::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) running = false;
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) running = false;
        if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            windowWidth = event.window.data1;
            windowHeight = event.window.data2;
            glViewport(0, 0, windowWidth, windowHeight);
        }
    }
}

void Spdl::update(float deltaTime) {
    // Логика обновления
}
// ===== ЗАПОЛНЕННЫЙ ПРЯМОУГОЛЬНИК =====
void Spdl::DrawFillRect2d(int x, int y, int w, int h, Color color) {
    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + w, y);
    glVertex2i(x + w, y + h);
    glVertex2i(x, y + h);
    glEnd();
}

// ===== ЗАПОЛНЕННЫЙ КРУГ =====
void Spdl::DrawFillCircle2d(int cx, int cy, int radius, Color color) {
    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2i(cx, cy); // центр

    int segments = 32;
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.14159f * i / segments;
        float x = cx + cos(angle) * radius;
        float y = cy + sin(angle) * radius;
        glVertex2f(x, y);
    }
    glEnd();
}

// ===== ЗАПОЛНЕННЫЙ ТРЕУГОЛЬНИК =====
void Spdl::DrawFillTriangle2d(int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_TRIANGLES);
    glVertex2i(x1, y1);
    glVertex2i(x2, y2);
    glVertex2i(x3, y3);
    glEnd();
}