#include "StandartOpenGlRender.hpp"
#include <iostream>
#include <GL/glew.h>

StandartOpenGlRender::StandartOpenGlRender() {}

StandartOpenGlRender::~StandartOpenGlRender() {
    if (glContext) SDL_GL_DestroyContext(glContext);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool StandartOpenGlRender::init(int width, int height, const char* title) {
    windowWidth = width;
    windowHeight = height;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow(
        title,
        windowWidth, windowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        std::cout << "Window Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    glContext = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Камера немного дальше от сцены
    camera.setPosition(vec3(0, 3, 15));
    camera.lookAt(vec3(0, 0, 0));

    // Инициализируем yaw/pitch по текущему направлению камеры
    {
        const Transform& t = camera.getTransform();
        vec3 dir = t.forward().normalize();
        cameraYaw = std::atan2(dir.x, dir.z);
        cameraPitch = std::asin(-dir.y);
    }

    lastTime = SDL_GetTicks() / 1000.0f;

    return true;
}

void StandartOpenGlRender::run() {
    while (running) {
        float currentTime = SDL_GetTicks() / 1000.0f;
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // FPS подсчет
        frameCount++;
        if (deltaTime > 0) {
            fps = (int)(1.0f / deltaTime);
        }

        // События
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) running = false;
            if (event.type == SDL_EVENT_WINDOW_RESIZED) {
                windowWidth = event.window.data1;
                windowHeight = event.window.data2;
                glViewport(0, 0, windowWidth, windowHeight);
            }

            // Управление мышью: удерживаем правую кнопку для обзора
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                event.button.button == SDL_BUTTON_RIGHT) {
                rightMouseDown = true;
                SDL_SetWindowRelativeMouseMode(window, true);
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP &&
                event.button.button == SDL_BUTTON_RIGHT) {
                rightMouseDown = false;
                SDL_SetWindowRelativeMouseMode(window, false);
            }
            if (event.type == SDL_EVENT_MOUSE_MOTION && rightMouseDown) {
                float dx = (float)event.motion.xrel * mouseSensitivity;
                float dy = (float)event.motion.yrel * mouseSensitivity;

                // Накапливаем углы
                cameraYaw   += -dx;
                cameraPitch += -dy;

                // Ограничиваем наклон вверх/вниз
                const float maxPitch = 1.3f; // ~75 градусов
                if (cameraPitch > maxPitch)  cameraPitch = maxPitch;
                if (cameraPitch < -maxPitch) cameraPitch = -maxPitch;

                // Строим кватернион из yaw/pitch
                quat qPitch = quat::fromAxisAngle(vec3(1, 0, 0), cameraPitch);
                quat qYaw   = quat::fromAxisAngle(vec3(0, 1, 0), cameraYaw);
                camera.getTransform().rotation = qYaw * qPitch;
            }
        }

        // Движение камеры (WASD + SPACE / CTRL) в системе координат камеры
        const bool* keys = SDL_GetKeyboardState(nullptr);
        vec3 moveDir(0, 0, 0);

        Transform& camT = camera.getTransform();
        vec3 forward = camT.forward(); // куда смотрим
        vec3 right   = camT.right();   // вправо от камеры

        if (keys[SDL_SCANCODE_W]) moveDir += forward;
        if (keys[SDL_SCANCODE_S]) moveDir -= forward;
        if (keys[SDL_SCANCODE_A]) moveDir -= right;
        if (keys[SDL_SCANCODE_D]) moveDir += right;
        if (keys[SDL_SCANCODE_SPACE])   moveDir.y += 1.0f;
        if (keys[SDL_SCANCODE_LCTRL])   moveDir.y -= 1.0f;

        if (moveDir.length() > 0.0f) {
            moveDir = moveDir.normalize() * cameraMoveSpeed * deltaTime;
            camT.position = camT.position + moveDir;
        }

        // Обновление объектов
        for (auto& obj : sceneObjects) {
            obj->update(deltaTime);
        }

        // Рендер
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float aspect = (float)windowWidth / windowHeight;
        mat4 projection = camera.getProjectionMatrix(aspect);
        glMultMatrixf(projection.ptr());

        glMatrixMode(GL_MODELVIEW);
        glLoadMatrixf(camera.getViewMatrix().ptr());

        for (auto& obj : sceneObjects) {
            glPushMatrix();
            glMultMatrixf(obj->transform.getMatrix().ptr());
            obj->render();
            glPopMatrix();
        }

        SDL_GL_SwapWindow(window);
        SDL_Delay(10);
    }
}

void StandartOpenGlRender::stop() {
    running = false;
}