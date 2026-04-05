#pragma once
#include "vec2.hpp"
#include "SimpleSDL.hpp"
#include <SDL3/SDL.h>

class Input {
private:
    bool LockMause = false;       
    vec2 movecameraoffser;          

public:
    void SetLockMause(SDL_Window* window, int width, int height) {
        LockMause = !LockMause;
        if (LockMause) {
            SDL_WarpMouseInWindow(window, width / 2, height / 2);
        }
    }

    int MauseOffset() { 
        return 0;
    }

    bool isMouseLocked() const { return LockMause; }
    vec2 getMoveOffset() const { return movecameraoffser; }
};