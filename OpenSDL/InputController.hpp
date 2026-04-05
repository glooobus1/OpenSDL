#pragma once
#include <SDL3/SDL.h>
#include <iostream>
#include <cmath>

class InputController {
private:
    SDL_Gamepad* m_gamepad;
    bool m_connected;

    float m_axes[6];
    bool m_currentButtons[15];
    bool m_previousButtons[15];

    float normalizeAxis(Sint16 value) const {
        return (value < 0) ? value / 32768.0f : value / 32767.0f;
    }

public:
    InputController() : m_gamepad(nullptr), m_connected(false) {
        for (int i = 0; i < 6; i++) m_axes[i] = 0.0f;
        for (int i = 0; i < 15; i++) {
            m_currentButtons[i] = false;
            m_previousButtons[i] = false;
        }
    }

    ~InputController() {
        if (m_gamepad) {
            SDL_CloseGamepad(m_gamepad);
            m_gamepad = nullptr;
        }
    }

    bool init() {
        if (!SDL_WasInit(SDL_INIT_GAMEPAD)) {
            SDL_InitSubSystem(SDL_INIT_GAMEPAD);
        }

        int numGamepads = 0;
        SDL_JoystickID* gamepadIDs = SDL_GetGamepads(&numGamepads);

        if (gamepadIDs && numGamepads > 0) {
            m_gamepad = SDL_OpenGamepad(gamepadIDs[0]);
            if (m_gamepad) {
                m_connected = true;
                printf("Gamepad connected: %s\n", SDL_GetGamepadName(m_gamepad));
                return true;
            }
        }

        printf("No gamepad found!\n");
        return false;
    }
    void update() {
        // ❗ ОБЯЗАТЕЛЬНО: обновляем события
        SDL_PumpEvents();

        if (!m_connected) return;

        // Сохраняем предыдущее состояние кнопок
        for (int i = 0; i < 15; i++) {
            m_previousButtons[i] = m_currentButtons[i];
        }

        // Оси
        m_axes[0] = normalizeAxis(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFTX));
        m_axes[1] = normalizeAxis(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFTY));
        m_axes[2] = normalizeAxis(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_RIGHTX));
        m_axes[3] = normalizeAxis(SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_RIGHTY));

        // Триггеры (обычно 0..1)
        m_axes[4] = SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_LEFT_TRIGGER) / 32767.0f;
        m_axes[5] = SDL_GetGamepadAxis(m_gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER) / 32767.0f;

        // Кнопки
        m_currentButtons[0] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_SOUTH);
        m_currentButtons[1] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_EAST);
        m_currentButtons[2] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_WEST);
        m_currentButtons[3] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_NORTH);
        m_currentButtons[4] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
        m_currentButtons[5] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);
        m_currentButtons[6] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP);
        m_currentButtons[7] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN);
        m_currentButtons[8] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT);
        m_currentButtons[9] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
        m_currentButtons[10] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_START);
        m_currentButtons[11] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_BACK);
        m_currentButtons[12] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_LEFT_STICK);
        m_currentButtons[13] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_RIGHT_STICK);
        m_currentButtons[14] = SDL_GetGamepadButton(m_gamepad, SDL_GAMEPAD_BUTTON_GUIDE);
    }

    bool isConnected() const { return m_connected; }

    float getAxis(int axis) const {
        if (axis < 0 || axis >= 6) return 0.0f;
        return m_axes[axis];
    }

    bool getButton(int button) const {
        if (button < 0 || button >= 15) return false;
        return m_currentButtons[button];
    }

    bool getButtonDown(int button) const {
        if (button < 0 || button >= 15) return false;
        return m_currentButtons[button] && !m_previousButtons[button];
    }

    bool getButtonUp(int button) const {
        if (button < 0 || button >= 15) return false;
        return !m_currentButtons[button] && m_previousButtons[button];
    }

    enum Axis {
        LeftX = 0,
        LeftY,
        RightX,
        RightY,
        LeftTrigger,
        RightTrigger
    };

    enum Button {
        A = 0,
        B,
        X,
        Y,
        LeftShoulder,
        RightShoulder,
        DPadUp,
        DPadDown,
        DPadLeft,
        DPadRight,
        Start,
        Back,
        LeftStick,
        RightStick,
        Guide
    };
};