#include <iostream>
#include <windows.h>
#include "SoundSystem.hpp"

int main() {
    SoundSystem audio;

    if (!audio.init()) {
        std::cerr << "Ошибка инициализации!" << std::endl;
        return -1;
    }

    // Просто указываем относительный путь!
    audio.loadSound("test", audio.setPath("saunds\\edge\\StartEdge.wav"));
    // Или так: audio.loadSound("test", audio.setPath("saunds/edge/StartEdge.wav"));

    std::cout << "Воспроизведение звука..." << std::endl;
    int id = audio.play("test", false, 1.0f);

    while (audio.isPlaying(id)) {
        audio.update();
        Sleep(100);
    }

    std::cout << "Звук закончился!" << std::endl;

    return 0;
}